#ifndef DIAGNOSTIC_AGENT_H
#define DIAGNOSTIC_AGENT_H

#include <string>
#include <vector>
#include <chrono>

// Forward declaration for bitnet structures to avoid heavy header dependency here
struct ggml_bitnet_context;

class DiagnosticAgent
{
public:
    DiagnosticAgent();
    ~DiagnosticAgent();

    bool loadModel(const std::string& modelPath);
    std::string processObservation(const std::string& jsonInput);

    // Performance metrics getters
    double getLastTTFT() const;
    double getLastTPS() const;

private:
    // Internal helper for prompt formatting
    std::string formatPrompt(const std::string& observation);

    // Placeholder for BitNet context
    // In a real implementation this would hold the model state
    void* m_modelContext; // Void* for decoupling in this demo

    // Metrics state
    double m_lastTTFT;
    double m_lastTPS;
};

#endif // DIAGNOSTIC_AGENT_H
