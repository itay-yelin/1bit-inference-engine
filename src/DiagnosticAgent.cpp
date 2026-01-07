#include "DiagnosticAgent.h"
#include "../external/bitnet/3rdparty/llama.cpp/include/llama.h"
#include <cstring>
#include <iostream>
#include <sstream>


DiagnosticAgent::DiagnosticAgent()
    : m_model(nullptr), m_lastTTFT(0.0), m_lastTPS(0.0) {
  llama_backend_init();
}

DiagnosticAgent::~DiagnosticAgent() {
  if (m_model) {
    llama_free_model(m_model);
  }
  llama_backend_free();
}

bool DiagnosticAgent::loadModel(const std::string &modelPath) {
  std::cout << "Loading 1-bit model from: " << modelPath << "..." << std::endl;

  llama_model_params model_params = llama_model_default_params();
  // Explicitly using CPU as this is a BitNet CPU demo
  model_params.n_gpu_layers = 0;

  m_model = llama_load_model_from_file(modelPath.c_str(), model_params);

  if (m_model == NULL) {
    std::cerr << "Error: unable to load model from " << modelPath << std::endl;
    return false;
  }

  std::cout << "Model loaded successfully (Real BitNet Backend)." << std::endl;
  return true;
}

std::string DiagnosticAgent::formatPrompt(const std::string &observation) {
  std::stringstream ss;
  ss << "Instruction: Analyze the following visual defect data and provide a "
        "technician summary.\n";
  ss << "Input: " << observation << "\n";
  ss << "Response: ";
  return ss.str();
}

std::string DiagnosticAgent::processObservation(const std::string &jsonInput) {
  if (!m_model) {
    return "Error: Model not loaded.";
  }

  std::string prompt = formatPrompt(jsonInput);

  // Tokenize
  const int n_prompt = -llama_tokenize(m_model, prompt.c_str(), prompt.size(),
                                       NULL, 0, true, true);
  std::vector<llama_token> prompt_tokens(n_prompt);
  if (llama_tokenize(m_model, prompt.c_str(), prompt.size(),
                     prompt_tokens.data(), prompt_tokens.size(), true,
                     true) < 0) {
    return "Error: Failed to tokenize prompt.";
  }

  // Init context
  llama_context_params ctx_params = llama_context_default_params();
  int n_predict = 128; // Set a reasonable limit for the technician summary
  ctx_params.n_ctx = n_prompt + n_predict;
  ctx_params.n_batch = n_prompt;
  ctx_params.no_perf = false;

  llama_context *ctx = llama_new_context_with_model(m_model, ctx_params);
  if (!ctx) {
    return "Error: Failed to create context.";
  }

  // Init sampler
  auto sparams = llama_sampler_chain_default_params();
  llama_sampler *smpl = llama_sampler_chain_init(sparams);
  llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

  // Prepare batch for the prompt
  llama_batch batch =
      llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size(), 0, 0);

  auto start = std::chrono::high_resolution_clock::now();

  int n_decode = 0;
  std::string outputResponse;
  llama_token new_token_id;

  bool firstToken = true;
  auto firstTokenTime = start; // Will be updated on first generated token

  // Generation loop
  // Note: n_prompt + n_predict limits the total size
  for (int n_pos = 0; n_pos + batch.n_tokens < ctx_params.n_ctx;) {
    // Evaluate tokens
    if (llama_decode(ctx, batch)) {
      outputResponse += " [Error: Decode Failed]";
      break;
    }
    n_pos += batch.n_tokens;

    // Sample next token
    new_token_id = llama_sampler_sample(smpl, ctx, -1);

    // Check for End of Generation
    if (llama_token_is_eog(m_model, new_token_id)) {
      break;
    }

    // Record TTFT on first token
    if (firstToken) {
      firstTokenTime = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> ttft_duration =
          firstTokenTime - start;
      m_lastTTFT = ttft_duration.count();
      firstToken = false;
    }

    // Convert token to string and append
    char buf[128];
    int n =
        llama_token_to_piece(m_model, new_token_id, buf, sizeof(buf), 0, true);
    if (n < 0) {
      // Failed to convert, skip
    } else {
      outputResponse += std::string(buf, n);
    }

    // Prepare next batch with the generated token
    batch = llama_batch_get_one(&new_token_id, 1, n_pos, 0);
    n_decode++;
  }

  auto endTime = std::chrono::high_resolution_clock::now();

  if (firstToken) {
    // If no tokens generated, set TTFT to 0
    m_lastTTFT = 0.0;
    firstTokenTime = endTime; // avoid negative duration
  }

  // Calculate TPS (tokens / generation time)
  std::chrono::duration<double> generation_duration = endTime - firstTokenTime;
  if (generation_duration.count() > 0 && n_decode > 0) {
    m_lastTPS = n_decode / generation_duration.count();
  } else {
    m_lastTPS = 0.0;
  }

  // Cleanup
  llama_sampler_free(smpl);
  llama_free(ctx);

  return outputResponse;
}
