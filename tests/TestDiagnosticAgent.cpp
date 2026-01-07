#include "../src/DiagnosticAgent.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>


// STRICT FORMATTING: All braces on new lines

void assertContains(const std::string &actual, const std::string &expected,
                    const std::string &testName) {
  if (actual.find(expected) != std::string::npos) {
    std::cout << "[PASS] " << testName << std::endl;
  } else {
    std::cerr << "[FAIL] " << testName << std::endl;
    std::cerr << "   Expected to find: \"" << expected << "\"" << std::endl;
    std::cerr << "   Actual: \"" << actual << "\"" << std::endl;
    exit(1);
  }
}

void testTireDefect() {
  DiagnosticAgent agent;
  agent.loadModel("mock_path");

  std::string input =
      "{\"detected_object\": \"tire_sidewall\", \"defect\": \"bulge\"}";
  std::string output = agent.processObservation(input);

  assertContains(output, "Critical Warning", "Tire Defect Detection");
}

void testMinorScratch() {
  DiagnosticAgent agent;
  agent.loadModel("mock_path");

  std::string input =
      "{\"detected_object\": \"panel\", \"defect\": \"scratch\"}";
  std::string output = agent.processObservation(input);

  assertContains(output, "Minor cosmetic scratch", "Minor Scratch Detection");
}

void testMetricsRecorded() {
  DiagnosticAgent agent;
  agent.loadModel("mock_path");

  std::string input =
      "{\"detected_object\": \"tire_sidewall\", \"defect\": \"bulge\"}";
  agent.processObservation(input);

  if (agent.getLastTTFT() > 0.0) {
    std::cout << "[PASS] TTFT Recorded (" << agent.getLastTTFT() << " ms)"
              << std::endl;
  } else {
    std::cerr << "[FAIL] TTFT was 0.0" << std::endl;
    exit(1);
  }

  // TPS might be 0 if string is empty, but our mock always returns something
  if (agent.getLastTPS() > 0.0) {
    std::cout << "[PASS] TPS Recorded (" << agent.getLastTPS() << " t/s)"
              << std::endl;
  } else {
    std::cerr << "[FAIL] TPS was 0.0" << std::endl;
    exit(1);
  }
}

int main() {
  std::cout << "Running DiagnosticAgent Tests..." << std::endl;
  std::cout << "--------------------------------" << std::endl;

  testTireDefect();
  testMinorScratch();
  testMetricsRecorded();

  std::cout << "--------------------------------" << std::endl;
  std::cout << "All tests passed successfully." << std::endl;
  return 0;
}
