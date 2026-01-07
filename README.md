# Semantic Reasoner (1-Bit LLM Demo)

A portfolio project demonstrating Edge AI capabilities using `bitnet.cpp` (Microsoft's 1-bit LLM framework). This application runs a "Semantic Reasoner" on the CPU to analyze simulated computer vision defects.

## 1. Setup

### External Dependencies
**Crucial Step**: You must properly clone the `bitnet` repository with submodules.

```bash
mkdir external
cd external
git clone --recursive https://github.com/microsoft/BitNet.git bitnet
cd ..
```

### Build C++ Backend

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 2. Usage

### Command Line Interface

Run the demo loop (Process 5 scenarios):

```bash
# Windows
.\Release\SemanticReasoner.exe --demo

# Linux/Mac
./SemanticReasoner --demo
```

Run a single prediction (process specific JSON):

```bash
# Windows
.\Release\SemanticReasoner.exe --predict "{\"detected_object\": \"tire\", \"defect\": \"puncture\", \"confidence\": 0.99}"
```

### Running Unit Tests (New)

Validate the agent's logic and metric recording before deployment.

```bash
# Run the test executable directly
.\Release\TestSemanticReasoner.exe
```

## 3. Streamlit UI

Run the interactive dashboard:

```bash
pip install streamlit
streamlit run ui/app.py
```
