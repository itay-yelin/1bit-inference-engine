import streamlit as st
import subprocess
import json
import os
import sys

# ------------------------------------------------------------------------------
# HELPER: Executable Auto-Detection
# ------------------------------------------------------------------------------
def find_executable(name):
    """
    Attempts to locate the C++ executable in common CMake build directories.
    """
    possible_paths = [
        f"build/Release/{name}.exe",       # Windows MSVC Release
        f"build/Debug/{name}.exe",         # Windows MSVC Debug
        f"build/{name}",                   # Linux/Mac Standard
        f"build/src/{name}",               # Linux/Mac Subdir
        f"../build/Release/{name}.exe",    # Relative check
    ]
    
    for path in possible_paths:
        if os.path.exists(path):
            return path
    
    # Fallback to a default string if not found
    return "build/Release/SemanticReasoner.exe"

# ------------------------------------------------------------------------------
# APP CONFIGURATION
# ------------------------------------------------------------------------------
st.set_page_config(page_title="Semantic Reasoner (bitnet.cpp)", page_icon="🧠", layout="wide")

st.markdown("""
<style>
    .reportview-container { background: #f0f2f6 }
    .big-font { font-size:20px !important; font-family: 'Courier New', monospace; }
</style>
""", unsafe_allow_html=True)

st.title("Edge AI Semantic Reasoner")
st.markdown("**Powered by bitnet.cpp (1-bit LLM) on CPU**")

# ------------------------------------------------------------------------------
# SIDEBAR
# ------------------------------------------------------------------------------
st.sidebar.header("Configuration")

# Auto-detect path to help the user
default_path = find_executable("SemanticReasoner")
executable_path = st.sidebar.text_input("Path to Executable", value=default_path)

# Scenarios
scenarios = {
    "Scenario 1: Tire Bulge": {
        "detected_object": "tire_sidewall", 
        "defect": "bulge", 
        "confidence": 0.95, 
        "severity_score": 8
    },
    "Scenario 2: Paint Scratch": {
        "detected_object": "panel_hood", 
        "defect": "scratch", 
        "confidence": 0.88, 
        "severity_score": 2
    },
    "Scenario 3: Windshield Crack": {
        "detected_object": "windshield", 
        "defect": "crack", 
        "confidence": 0.99, 
        "severity_score": 9
    },
    "Scenario 4: Custom": {}
}

selected_scenario_name = st.sidebar.selectbox("Select Test Scenario", list(scenarios.keys()))

if selected_scenario_name == "Scenario 4: Custom":
    custom_json = st.sidebar.text_area("Custom JSON Input", value='{"detected_object": "unknown", "defect": "odd_texture", "severity_score": 5}')
    try:
        input_data = json.loads(custom_json)
    except:
        st.error("Invalid JSON")
        input_data = None
else:
    input_data = scenarios[selected_scenario_name]

# ------------------------------------------------------------------------------
# MAIN EXECUTION
# ------------------------------------------------------------------------------
col1, col2 = st.columns(2)

with col1:
    st.subheader("Compute Vision Input")
    st.info("Simulated Detector Output")
    st.json(input_data)

    if st.button("RUN INFERENCE", type="primary"):
        if not os.path.exists(executable_path):
            st.error(f"Executable not found at: {executable_path}")
            st.warning("Please build the C++ project first! (See README.md)")
        else:
            with st.spinner("Running 1-bit LLM Inference..."):
                try:
                    # serialize json for command line
                    json_arg = json.dumps(input_data)
                    
                    cmd = [executable_path, "--predict", json_arg]
                    
                    process = subprocess.run(
                        cmd, 
                        capture_output=True, 
                        text=True,
                        check=True
                    )
                    
                    output_str = process.stdout.strip()
                    
                    # Robust parsing: Look for the JSON object within the stdout
                    # (Handles cases where backend might print initialization logs)
                    json_start = output_str.find('{')
                    json_end = output_str.rfind('}') + 1
                    
                    if json_start != -1 and json_end != -1:
                        final_json = output_str[json_start:json_end]
                        result = json.loads(final_json)
                        st.session_state['last_result'] = result
                    else:
                        st.error("Failed to parse output from C++ backend.")
                        st.text("Raw Output:")
                        st.code(output_str)

                except subprocess.CalledProcessError as e:
                    st.error(f"Execution failed with code {e.returncode}")
                    st.text("Stderr:")
                    st.code(e.stderr)
                except Exception as e:
                    st.error(f"Error: {e}")

with col2:
    st.subheader("Semantic Reasoner Output")
    
    if 'last_result' in st.session_state:
        res = st.session_state['last_result']
        
        st.success("Analysis Complete")
        
        st.markdown("### Technician Summary")
        # Use .get() for safety
        st.markdown(f"> {res.get('summary', 'No summary provided')}")
        
        st.markdown("---")
        st.markdown("### Performance Metrics (CPU)")
        
        m1, m2 = st.columns(2)
        ttft = res.get('ttft_ms', 0)
        tps = res.get('tps', 0)
        
        m1.metric("Time To First Token", f"{ttft:.2f} ms")
        m2.metric("Tokens Per Second", f"{tps:.2f} t/s")
        
        st.caption("1-bit quantization enables high-speed inference on standard hardware.")

    else:
        st.write("Waiting for analysis...")

