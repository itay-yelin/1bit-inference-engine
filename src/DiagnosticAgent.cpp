#include "DiagnosticAgent.h"
#include <iostream>
#include <sstream>
#include <thread>

// Mock include for bitnet API - in real usage this would be #include
// "ggml-bitnet.h" We will simulate the latency and output for the portfolio
// demo purposes if the actual model library isn't linked, but the structure is
// ready.

DiagnosticAgent::DiagnosticAgent()
    : m_modelContext(nullptr), m_lastTTFT(0.0), m_lastTPS(0.0) {}

DiagnosticAgent::~DiagnosticAgent() {
  // Cleanup if context exists
}

bool DiagnosticAgent::loadModel(const std::string &modelPath) {
  // Simulate model loading
  std::cout << "Loading 1-bit model from: " << modelPath << "..." << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::cout << "Model loaded (Simulated BitNet Backend)." << std::endl;
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
  // Start timing for Time To First Token (TTFT)
  auto start = std::chrono::high_resolution_clock::now();

  std::string prompt = formatPrompt(jsonInput);

  // Simulate Inference Latency (TTFT)
  // In a real CPU 1-bit model, this might be 10-50ms depending on hardware
  std::this_thread::sleep_for(std::chrono::milliseconds(15));

  auto firstTokenTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ttft_duration =
      firstTokenTime - start;
  m_lastTTFT = ttft_duration.count();

  // Determine output based on simplistic keywords in input
  std::string outputResponse;
  if (jsonInput.find("tire_sidewall") != std::string::npos &&
      jsonInput.find("bulge") != std::string::npos) {
    outputResponse =
        "Critical Warning: High-confidence structural defect detected on tire "
        "sidewall. Immediate replacement recommended.";
  } else if (jsonInput.find("scratch") != std::string::npos) {
    outputResponse = "Minor cosmetic scratch detected. No structural "
                     "compromise. Continue monitoring.";
  } else if (jsonInput.find("crack") != std::string::npos) {
    outputResponse = "Structural crack detected. Severity is high. Scheduling "
                     "detailed inspection.";
  } else if (jsonInput.find("dent") != std::string::npos) {
    outputResponse = "Surface dent observed. Check against tolerance limits.";
  } else {
    outputResponse = "Anomaly detected. Please review manual inspection logs.";
  }

  // Simulate generation time (Tokens Per Second)
  // Assume roughly 1 token per 4 characters
  int tokenCount = outputResponse.length() / 4;
  if (tokenCount == 0) {
    tokenCount = 1;
  }

  // Simulate generation delay
  int generationTimeMs = tokenCount * 20;
  std::this_thread::sleep_for(std::chrono::milliseconds(generationTimeMs));

  auto endTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> total_duration = endTime - firstTokenTime;

  // Calculate TPS: tokens / seconds
  if (total_duration.count() > 0) {
    m_lastTPS = tokenCount / total_duration.count();
  } else {
    m_lastTPS = 0.0;
  }

  return outputResponse;
}

double DiagnosticAgent::getLastTTFT() const { return m_lastTTFT; }

double DiagnosticAgent::getLastTPS() const { return m_lastTPS; }
