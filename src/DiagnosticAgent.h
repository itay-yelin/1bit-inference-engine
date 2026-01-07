#ifndef DIAGNOSTIC_AGENT_H
#define DIAGNOSTIC_AGENT_H

#include <chrono>
#include <string>
#include <vector>


// Forward declaration
struct llama_model;

class DiagnosticAgent {
public:
  DiagnosticAgent();
  ~DiagnosticAgent();

  bool loadModel(const std::string &modelPath);
  std::string processObservation(const std::string &jsonInput);

  // Performance metrics getters
  double getLastTTFT() const;
  double getLastTPS() const;

private:
  // Internal helper for prompt formatting
  std::string formatPrompt(const std::string &observation);

  // Real Llama model pointer
  struct llama_model *m_model;

  // Metrics state
  double m_lastTTFT;
  double m_lastTPS;
};

#endif // DIAGNOSTIC_AGENT_H
