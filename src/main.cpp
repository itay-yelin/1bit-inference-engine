#include "DiagnosticAgent.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <thread>
#include <vector>


// STRICT FORMATTING: All braces on new lines

struct SimulationScenario {
  std::string inputJson;
  std::string description;
};

// Helper to escape JSON strings to prevent UI parsing errors
std::string escapeJsonString(const std::string &input) {
  std::string output;
  output.reserve(input.length());
  for (char c : input) {
    if (c == '"') {
      output += "\\\"";
    } else if (c == '\\') {
      output += "\\\\";
    } else if (c == '\n') {
      output += "\\n";
    } else {
      output += c;
    }
  }
  return output;
}

std::vector<SimulationScenario> generateScenarios() {
  std::vector<SimulationScenario> scenarios;

  scenarios.push_back(
      {"{\"detected_object\": \"tire_sidewall\", \"defect\": \"bulge\", "
       "\"confidence\": 0.95, \"severity_score\": 8}",
       "Critical Tire Defect"});

  scenarios.push_back(
      {"{\"detected_object\": \"panel_hood\", \"defect\": \"scratch\", "
       "\"confidence\": 0.88, \"severity_score\": 2}",
       "Minor Paint Scratch"});

  scenarios.push_back(
      {"{\"detected_object\": \"windshield\", \"defect\": \"crack\", "
       "\"confidence\": 0.99, \"severity_score\": 9}",
       "Windshield Crack"});

  scenarios.push_back({"{\"detected_object\": \"bumper_front\", \"defect\": "
                       "\"dent\", \"confidence\": 0.75, \"severity_score\": 4}",
                       "Bumper Dent"});

  scenarios.push_back(
      {"{\"detected_object\": \"engine_mount\", \"defect\": \"missing_bolt\", "
       "\"confidence\": 0.92, \"severity_score\": 10}",
       "Missing Component"});

  return scenarios;
}

void runDemo(DiagnosticAgent &agent) {
  auto scenarios = generateScenarios();
  int count = 1;

  for (const auto &scenario : scenarios) {
    std::cout << "--- Scenario " << count++ << ": " << scenario.description
              << " ---" << std::endl;
    std::cout << "Input: " << scenario.inputJson << std::endl;

    std::string result = agent.processObservation(scenario.inputJson);

    std::cout << "Agent Summary: " << result << std::endl;
    std::cout << "Metrics -> TTFT: " << agent.getLastTTFT()
              << " ms | TPS: " << agent.getLastTPS() << " t/s" << std::endl;
    std::cout << std::endl;
  }
}

void runPrediction(DiagnosticAgent &agent, const std::string &inputJson) {
  std::string summary = agent.processObservation(inputJson);
  double ttft = agent.getLastTTFT();
  double tps = agent.getLastTPS();

  // Escape the summary to ensure valid JSON output
  std::string escapedSummary = escapeJsonString(summary);

  // Output JSON for UI parsing
  std::cout << "{"
            << "\"summary\": \"" << escapedSummary << "\", "
            << "\"ttft_ms\": " << ttft << ", "
            << "\"tps\": " << tps << "}" << std::endl;
}

int main(int argc, char *argv[]) {
  DiagnosticAgent agent;
  if (!agent.loadModel("models/1bit_model.gguf")) {
    std::cerr << "Failed to load model." << std::endl;
    return 1;
  }

  if (argc > 1) {
    std::string arg1 = argv[1];
    if (arg1 == "--demo") {
      runDemo(agent);
    } else if (arg1 == "--predict" && argc > 2) {
      std::string inputJson = argv[2];
      runPrediction(agent, inputJson);
    } else {
      std::cout
          << "Usage: SemanticReasoner.exe --demo OR --predict <json_string>"
          << std::endl;
    }
  } else {
    // Default to demo if no args
    runDemo(agent);
  }

  return 0;
}
