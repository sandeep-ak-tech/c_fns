# PSOC™ Edge MCU: DEEPCRAFT™ Deploy Vision — Interview Prep

## Layer 1: Overview

### Elevator Pitch
I built a real-time vision system that runs an object detection AI model entirely on a microcontroller. It captures live video from a USB camera, runs neural network
inference on a Cortex-M55, and draws bounding boxes on a display — all within the power and memory
envelope of an embedded MCU. Cortex M55 runs at 400Mhz. The real-time hand gesture detection is the key objective (rock, paper, or scissors).

---
### Hardware Used
**PSOC Edge:** Its a Cortex M Microcontroller. It has two cores CM33 and CM55. Cortex M55 runs at 400Mhz and with a U55 Co-processor. Arm Ethos-U55 is a dedicated NPU (Neural Processing Unit).

**Camera Interface:** We have interfaced a Logitech C290 Camera to the microcontroller . We have utilised USB Video class for live streaming and get image data for processing.

**Display Interface:** We have used a Waveshare 4.3 inch Graphics display using MIPI-DSI interface. Also the result was printed on a serial port.

### Implementation

The ML pipeline has three steps: Preprocessing, Inference, and Post-Processing.

#### Step 1: Preprocessing (in `draw()` — lcd_task.c)

The camera delivers raw **YUYV 422** at 320×240. Before the model can use it:

```
Camera (YUYV 422, 320×240, 2 bytes/pixel)
    │
    ▼  [GPU: vg_lite_blit] — "Blit" = Block Image Transfer
    │   GPU copies pixel block and converts color format in HW, zero CPU cycles
BGR565 (320×240, 2 bytes/pixel)
    │
    ▼  [GPU: mirror_image() — horizontal flip]
BGR565 mirrored
    │
    ├──► [GPU: vg_lite_blit with scale matrix 2.5×] → Display buffer (800×480)
    │         Purpose: show live camera feed on LCD for the user
    │
    └──► [SW: ifx_image_conv_RGB565_to_RGB888_i8()] → RGB888 uint8 (320×320×3)
              Purpose: prepare model input for neural network
              = 307,200 bytes, placed in SoCMEM (.cy_socmem_data)
```

**Why the fork (GPU path + SW path)?**
- **Display** needs a large scaled image (800×480) in BGR565 — GPU is built for scaling
- **Model** needs a 320×320 RGB888 uint8 image — VGLite GPU can't output RGB888 format, and the result must land in SoCMEM for the model's tensor input buffer
- One frame, two consumers (human eye + neural network) with different format requirements

**What is Blit?**
- Blit = **Bl**ock **I**mage **T**ransfer — a GPU operation that copies a rectangular block of pixels with optional HW transformations (color conversion, scaling, rotation)
- The VGLite GPU has its own internal DMA — CM55 just submits the blit command and is free until `vg_lite_flush()` completes

**DMA usage:**
- GPU blit steps use GPU-internal DMA (hardware accelerated, CPU free)
- The SW RGB conversion does NOT use DMA — it's a CPU loop on CM55, but data is in SoCMEM (single-cycle access), so it's still fast

**Key facts:**
- No normalization — raw uint8 values [0–248] go directly to the model (quantized TFLite model expects this)
- 320×240 is center-cropped/padded to **320×320** (model needs square input)
- Color extraction: `red = (pixel >> 8) & 0xF8`, `green = (pixel >> 3) & 0xFC`, `blue = (pixel << 3) & 0xF8`

---

#### Step 2: Inference (in `cm55_inference_task` — inference_task.c)

**Init (once at boot):**
```c
int result = IMAI_init();              // Load model, allocate tensor arena
IMAI_api_def *api_def = IMAI_api();   // Get model metadata (dimensions, classes)
```

**Per-frame (in loop):**
```c
uint8_t *image_buf_uint8 = get_image();    // calls draw() → preprocessing above
IMAI_compute(image_buf_uint8, data_out);   // RUN INFERENCE — this is the expensive step (~100ms)
```

**How TFLite Micro is invoked (call chain):**
```
IMAI_compute()
  → mtb_model_raw()       // DEEPCRAFT wrapper — copies input into tensor, triggers run
    → mtb_ml_model_run()  // ModusToolbox ML middleware — manages TFLite interpreter
      → TFLite Micro APIs (Invoke()) — runs each operator (Conv2D, DepthwiseConv, etc.)
                                        using CMSIS-NN optimized kernels on Helium
```
The app code only calls `IMAI_compute()`; the entire TFLite Micro runtime (interpreter, tensor allocation, kernel dispatch) is abstracted behind the DEEPCRAFT-generated wrapper.

**Model specs (from model.h):**

| Property | Value |
|---|---|
| Input | `uint8_t[320, 320, 3]` — 307,200 bytes |
| Output | `float[8, 5]` — 8 predictions × 5 attributes |
| Working buffer (RAM) | 357,600 bytes (SoCMEM) |
| Tensor arena (RAM) | 891,920 bytes (SoCMEM) |
| Model weights (Flash) | 1,722,352 bytes (~1.7 MB) |
| Backend | TensorFlow Lite for Microcontrollers |
| Compiler | ImagiNet (DEEPCRAFT) Compiler 5.7.3938 |

---

#### Step 3: Post-Processing (in `cm55_inference_task` — same file)

**What post-processing does:** The model outputs raw numbers (bounding box coordinates + class scores). Post-processing converts these raw values into human-readable results — it filters out invalid detections, finds the best class for each detection (argmax), converts normalized coordinates to pixel positions, and packages everything into a struct that the graphics task can use to draw boxes and labels on screen.

The model output `data_out[40]` is shaped as **[8 max_predictions × 5 attributes]** in attribute-major order:

```
Attribute 0: x_center (normalized 0–1)    × 8 slots
Attribute 1: y_center (normalized 0–1)    × 8 slots
Attribute 2: width (normalized 0–1)       × 8 slots
Attribute 3: height (normalized 0–1)      × 8 slots
Attribute 4: class_0 score (rock)         × 8 slots
Attribute 5: class_1 score (paper)        × 8 slots
Attribute 6: class_2 score (scissors)     × 8 slots
Attribute 7: detection_flag (0=invalid)   × 8 slots
```

**Logic:**
```c
for (int r = 0; r < max_predictions; r++) {
    if (data_out[detection_flag_index * max_predictions + r] == 0)
        break;   // stop at first non-detection

    // Extract normalized bounding box
    float x = data_out[0 * max_predictions + r];
    float y = data_out[1 * max_predictions + r];
    float w = data_out[2 * max_predictions + r];
    float h = data_out[3 * max_predictions + r];

    // Argmax over 3 classes → best class + confidence
    class_id[r] = get_best_class(class[r], NUM_CLASSES, &conf);

    // Convert normalized center/wh → pixel coordinates [xmin, ymin, xmax, ymax]
    xmin = (x - w/2) * IMAGE_WIDTH;
    ymin = (y - h/2) * IMAGE_WIDTH;
    xmax = (x + w/2) * IMAGE_WIDTH;
    ymax = (y + h/2) * IMAGE_WIDTH;
}
cy_rtos_semaphore_set(&model_semaphore);  // signal graphics task
```

The graphics task then scales these pixel coords from 320×320 model space → 800×480 display space and draws bounding boxes + labels. The graphics library is used to draw lines to make bounding boxes.

---

### How the Model files are generated?
```
model.c = Trained weights (THE model) + TFLite Micro runtime glue + memory buffers
model.h = Public API + size/type defines


```
Memory buffers : These are pre-allocated chunks of RAM that TFLite Micro uses during inference. he DEEPCRAFT Studio calculates exactly how much RAM each layer's output needs and generates these buffers with the right size.
```
CY_SECTION(".cy_socmem_data")  // placed in SoCMEM for fast single-cycle access
static ALIGNED(16) int8_t _buffer[357600];
```
"Public API" = int  IMAI_init(), void IMAI_compute()

The full ML workflow from raw data to deployed firmware:

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  1. Collect  │    │  2. Label   │    │  3. Create  │    │  4. Train   │
│  ~3900 imgs  │───►│  Draw bbox  │───►│  YOLO arch  │───►│  On PC/GPU  │
│  1300/class  │    │  + class    │    │  for MCU    │    │  FP32 model │
└─────────────┘    └─────────────┘    └─────────────┘    └──────┬──────┘
                                                                │
                   ┌─────────────┐    ┌─────────────┐    ┌──────▼──────┐
                   │  7. Deploy  │    │  6. CodeGen │    │  5. Test /  │
                   │  Flash MCU  │◄───│  model.c/.h │◄───│  Evaluate   │
                   │  Run infer  │    │  Quantize   │    │  Confusion  │
                   │  on CM55    │    │  INT8+Vela? │    │  Matrix     │
                   └─────────────┘    └─────────────┘    └─────────────┘
```

#### Step 1: Data Collection
- Used DEEPCRAFT™ Studio's built-in camera data collection tool (PC webcam or board camera)
- Captured images of three classes: **Rock**, **Paper**, and **Scissors**
- **Uniform dataset**: ~1300 images per class (total ~3900 images) — keeping classes balanced prevents the model from being biased toward one gesture

#### Step 2: Data Labeling
- For object detection, labeling means drawing a **bounding box** around the gesture in each image and assigning a **class label** (rock / paper / scissors)
- DEEPCRAFT™ Studio provides a labeling UI — you draw boxes on images and tag them
- Label format: `[class_id, x_center, y_center, width, height]` (normalized 0–1) — this is the **YOLO annotation format**
- Dataset is split into **Training set** (~80%) and **Test set** (~20%) within Studio

#### Step 3: Model Creation (Architecture Selection)
- DEEPCRAFT™ Studio uses a **YOLO-based architecture** internally (Ultralytics YOLO), optimized and scaled down for microcontroller targets
- **Why YOLO?** — It processes the entire image in a single forward pass (no region proposals, no two-stage pipeline), making it fast enough for real-time inference on constrained devices
- The architecture is sized based on target device constraints (RAM, flash, compute)

#### Step 4: Model Training
- Training runs on a **PC/cloud GPU** (not on the MCU) — DEEPCRAFT™ Studio handles this
- The model sees thousands of labeled images, adjusts its weights to minimize detection error
- Training parameters: epochs, batch size, learning rate — Studio provides sensible defaults
- Output: a trained model file (`.h5`) containing learned weights + architecture
- During training, the model is **FP32** (32-bit floating point) — full precision for best accuracy

#### Step 5: Model Evaluation / Testing
- Studio runs the model on test images it has never seen during training
- Metrics:
  - **Accuracy** — does it detect the right class?
  - **mAP (mean Average Precision)** — standard object detection metric, measures box overlap + class correctness
  - **Confusion Matrix** — shows which classes get mixed up (e.g., scissors confused for paper?)
- If accuracy is poor → go back to Step 1 (more data) or Step 3 (different architecture)

#### Step 6: Code Generation — Optimization & Quantization

DEEPCRAFT™ Studio converts the trained model into C code (`model.c` / `model.h`) using **TFLite Micro as the backend**.

**6a. Quantization (FP32 → INT8)**
- Convert all weights and activations from 32-bit floats to 8-bit integers
- **Why:** 4× smaller model, 4× less memory for activations, maps to Helium's 16-wide INT8 SIMD lanes
- **How:** Post-training quantization — Studio feeds calibration data (from the training set) through the model to determine min/max range of each tensor, then maps float values to int8 range [-128, 127]
- **Trade-off:** Small accuracy loss (~1-2%) but massive speed and size gains

**6b. Preprocessor Optimization**
- CMSIS-DSP library acceleration options: CMSIS Float32 (for boards with FPU), CMSIS Q15/Q31 (without FPU)

**6c. Sparsity**
- Optional: packs sparse (near-zero) weights to save flash memory

**6d. Vela Compiler (for CM55 + U55 targets only)**
- Used only when targeting PSOC Edge **M55+U55** (with Ethos-U55 NPU)
- Arm's offline compiler that further optimizes the quantized `.tflite` for U55 hardware
- Vela settings in Studio: Tensor Allocator (Hill Climb), System Config (SoCMEM 300/200 MHz), Optimize (Size/Performance), Memory Mode (Shared SRAM / SRAM Only)
- **In our project:** PSOC Edge doesn't have U55, so Vela is NOT used

**6e. Generated Output**
- `model.c` — model weights as C arrays (flash), tensor arena size definitions (SoCMEM)
- `model.h` — API declarations: `IMAI_init()`, `IMAI_compute()`, `IMAI_finalize()`, `IMAI_api()`
- Backend: DEEPCRAFT™ wraps TFLite Micro's interpreter — `IMAI_compute()` ultimately calls TFLite Micro's `Invoke()` which dispatches CMSIS-NN optimized kernels

#### Step 7: Model Deployment / Inferencing
- Copy generated `model.c` / `model.h` into `proj_cm55/model/` folder (Studio can do this directly via ModusToolbox™ integration)
- Update `NUM_CLASSES` and `MAX_PREDICTIONS` in `inference_task.h` if the model changed
- Build firmware, flash all three projects (CM33_S, CM33_NS, CM55) to the board
- At runtime:
  - `IMAI_init()` — loads model, allocates tensor arena in SoCMEM
  - `IMAI_compute(input, output)` — runs inference per frame
  - Post-processing extracts bounding boxes + labels → displayed on screen

---

### USB Camera Interface

**Protocol:** USB Video Class (UVC 1.1) — an industry-standard class driver for video streaming. The camera enumerates as a UVC-compliant device, so no custom/proprietary driver is needed — the host stack handles device discovery, format negotiation, and streaming natively.

**Implementation:** Reused an existing SEGGER emUSB-Host video streaming code example. The `cm55_usb_webcam_task` initializes the USB host stack and manages the full UVC lifecycle.

---

### Display in This Project

- The display shows the **live camera feed** with **bounding boxes** and **text labels** (rock/paper/scissors) overlaid when a gesture is detected.
- The `cm55_ns_gfx_task` waits for the inference task to finish (via `model_semaphore`), then:
  1. Takes the camera frame and scales it to fit the display using a transformation matrix
  2. Draws colored bounding boxes around detected gestures
  3. Draws the class label text (rock/paper/scissors) and inference time
  4. Calls `vg_lite_flush()` to push the rendered frame to the display
- Double buffering is used — one buffer is shown on screen while the other is being drawn to. Swap happens after `vg_lite_flush()` completes. No tearing.

---

### MIPI DSI — Short Note

- MIPI DSI = Mobile Industry Processor Interface, Display Serial Interface
- A high-speed serial link for sending pixel data from SoC to display
- Uses D-PHY (differential signaling): 1 clock lane + 1 or more data lanes, just 4 wires for 1-lane
- Two modes:
  - **LP (Low Power)** — used during init to send configuration commands to display controller
  - **HS (High Speed)** — used during runtime to stream pixels continuously
- This project uses **video mode** — the SoC continuously streams frames (display has no local frame buffer)
- Much faster than SPI, far fewer pins than parallel RGB

---

### PSOC Edge Graphics Subsystem

The PSOC Edge E84 has a dedicated hardware graphics block with three parts:

| Block | What It Does |
|---|---|
| **2.5D GPU (VGLite)** | Handles 2D operations: blit, scale, rotate, draw rectangles/lines. Offloads rendering from CM55. |
| **Display Controller (DC)** | Reads frame buffer from memory, generates timing signals, feeds pixels to MIPI DSI host |
| **MIPI DSI Host + D-PHY** | Serializes pixel data and sends it over differential lanes to the physical display |

**Key point:** The CM55 doesn't push pixels — it submits high-level draw commands to the GPU. The GPU renders into a frame buffer in memory, and the DC autonomously streams that buffer to the display. This means CM55 can start processing the next camera frame while the GPU is still rendering the previous result.

---

### FPS Performance Breakdown

| Configuration | FPS | Bottleneck |
|---|---|---|
| Camera only (streaming, no processing) | ~60 fps | USB bandwidth + sensor frame rate |
| Camera + Model inference (UART output) | ~10 fps | Inference time per frame (~100 ms) |
| Camera + Model + Graphics display | ~6 fps | Inference + GPU render + MIPI DSI refresh |

**Key insight:** The 10× drop from 60 → 6 fps is almost entirely due to inference compute, not camera or display — proving that CPU/model optimization is the critical path.

---

### Ethos-U55 & Vela Compiler (Not Used in This Project — But Good to Know)

**Ethos-U55:** A dedicated NPU (Neural Processing Unit) designed to pair with Cortex-M55. It accelerates ML inference by 10–50× over M55+Helium alone, handling Conv2D, DepthwiseConv, FC, Pooling in hardware.

**Vela Compiler:**
- Arm's **offline compiler** that optimizes a `.tflite` model specifically for the Ethos-U NPU
- Input: standard `.tflite` model → Output: optimized `.tflite` with Ethos-U custom operators
- What Vela does:
  - **Operator mapping** — decides which layers run on U55 vs fallback to M55
  - **Graph optimization** — fuses operators, reorders to minimize memory usage
  - **SRAM scheduling** — plans how intermediate tensors share the NPU's local SRAM (cascading)
  - **Command stream generation** — produces the binary instructions the U55 hardware executes directly
- If a layer isn't supported by U55 (e.g., custom op), Vela leaves it as-is → runs on M55 via TFLite Micro

**Why this matters for the interview:**
> "The PSOC Edge doesn't have an Ethos-U55 NPU, so I can't use the Vela compiler to offload ops to hardware. All inference runs purely on the Cortex-M55 CPU — which made CPU-level optimization (Helium SIMD, INT8 quantization, CMSIS-NN kernels, memory placement) absolutely critical to achieve usable frame rates."

---

### What is a Framework vs a Model?

- **Framework**: A software toolkit/library that provides the infrastructure to build, train, and run ML models.
  - Analogy: The kitchen (oven, pans, recipes).
  - In this project: TensorFlow / TFLite Micro is the framework — it provides the runtime to execute inference on the MCU.

- **Model**: A specific trained artifact (a file with weights + architecture) that takes input and produces predictions.
  - Analogy: The cooked dish.
  - In this project: The hand gesture detection model (rock/paper/scissors) exported from DEEPCRAFT™ Studio is the model — a `.tflite` file containing trained weights.

---

### TensorFlow
- Google's open-source ML framework for building and training neural networks.
- Runs on GPUs/CPUs on desktops and servers.
- Used (via DEEPCRAFT™ Studio) to train the model on hand gesture images.

### TFLite Micro (TensorFlow Lite for Microcontrollers)
- A stripped-down inference engine (~few hundred KB) designed for microcontrollers with limited RAM/flash (like Cortex-M55).
- It **cannot train** models — it only runs inference on a pre-trained `.tflite` model.
- In this project, the CM55 core runs TFLite Micro to execute gesture detection in real-time on camera frames.
- Key constraints: no dynamic memory allocation, no OS dependency, no filesystem — the model is compiled directly into firmware as a C array.

**Pipeline:**
TensorFlow (train on PC) → export as `.tflite` → quantize (INT8) → deploy via TFLite Micro on MCU

### YOLO (You Only Look Once)
- A family of object detection models (not a framework).
- Performs detection in a single pass through the network — hence "You Only Look Once" — making it very fast.
- Outputs bounding boxes + class labels + confidence scores for each detected object.
- The DEEPCRAFT™ model in this project is similar in purpose — it draws bounding boxes around rock/paper/scissors gestures in real-time video.
- Difference: this model is heavily optimized (quantized to INT8, reduced resolution) to fit within ~2 MB RAM constraints of Cortex-M55.

---


### Architecture Overview

```
┌──────────────────────────────────────────────────┐
│                  PSOC™ Edge E84                   │
│                                                   │
│  ┌───────────┐   Boot & Security   ┌───────────┐ │
│  │  CM33 (S) │ ──────────────────► │ CM33 (NS) │ │
│  │  Secure   │   TrustZone setup   │ Resource   │ │
│  │  Boot     │                     │ Init, then │ │
│  └───────────┘                     │ DeepSleep  │ │
│                                    └─────┬──────┘ │
│                                          │ Enable │
│                                          ▼        │
│                                    ┌───────────┐  │
│         USB Camera ──────────────► │  CM55     │  │
│                    video frames    │           │  │
│                                    │ 3 RTOS   │  │
│                                    │  Tasks:   │  │
│                                    │ • Webcam  │  │
│                                    │ • Infer   │  │
│                                    │ • Display │  │
│                                    └─────┬─────┘  │
│                                          │        │
│                        ┌─────────────────┘        │
│                        ▼                          │
│                  ┌──────────┐                     │
│                  │ 2.5D GPU │ ──► MIPI DSI ──► LCD│
│                  │ + DC     │   bounding boxes    │
│                  └──────────┘                     │
└──────────────────────────────────────────────────┘
```

**Three cores, three roles:**
- **CM33 Secure** — boot chain, root of trust, TrustZone security config
- **CM33 Non-Secure** — clock/pin/peripheral init, then **sleeps** (saves power)
- **CM55** — does all the real work: camera capture, ML inference, display rendering

---

### CPU Optimization Angle

#### 1. Helium/MVE — SIMD for Microcontrollers
- Cortex-M55 is the first M-profile core with Arm Helium (M-profile Vector Extension).
- Adds 128-bit vector processing: 4× INT32, 8× INT16, or **16× INT8 operations per cycle**.
- TFLite Micro + CMSIS-NN kernels exploit this to accelerate convolutions, depthwise separable convolutions, and fully-connected layers.
- Without Helium, inference would be 5–8× slower.

#### 2. INT8 Quantization — Compute Meets Memory
- Model is quantized from FP32 to INT8 (post-training quantization).
- 4× smaller model size.
- Maps directly to Helium's sweet spot: **16 INT8 MACs per vector instruction** vs 4 FP32 ops.
- Quantization is a co-optimization of the model and the CPU pipeline.

#### 3. Memory Placement Matters
- Model weights placed in **SoCMEM** (on-chip tightly-coupled memory) for best performance — not external flash.
- Tensor arena (scratch space for intermediate activations) also carefully placed to minimize access latency.
- Difference between SoCMEM and XIP from QSPI flash: **10–50× in access latency**.

#### 4. CMSIS-NN / CMSIS-DSP
- Inference uses CMSIS-NN optimized kernels hand-tuned for Helium — not naive C loops.
- These kernels handle:
  - Operator fusion (conv + bias + activation in one pass)
  - Loop unrolling
  - Vector tail predication (Helium's unique feature for handling non-power-of-2 dimensions without cleanup loops)

---

### Key Phrases to Use in Interview

- "We run inference at the edge with zero cloud dependency"
- "The model is INT8 quantized to exploit Helium's 16-wide SIMD lanes"
- "I placed weights in SoCMEM rather than XIP flash to cut memory access latency"
- "CMSIS-NN kernels fuse conv+bias+ReLU into single passes to maximize data reuse"
- "CM33 goes to DeepSleep after init — only CM55 is active during inference"
- "Helium's tail predication eliminates scalar cleanup loops for non-aligned tensor dimensions"

---

### Anticipated Q&A

**Q: Why not use a Linux SBC with a GPU?**
> Privacy, power, cost, and latency. This runs on a $10 MCU at milliwatts with deterministic latency.
> No boot time, no OS jitter, no network dependency. For always-on vision at the edge, an M-class MCU is the right fit.

**Q: What was the hardest optimization challenge?**
> Fitting the tensor arena in SRAM. Intermediate activations for conv layers can spike memory usage.
> I had to balance model architecture (fewer channels, smaller input resolution) against detection accuracy,
> and carefully place buffers in SoCMEM vs external memory.

**Q: How does Helium compare to a full Neon unit?**
> Helium is designed for M-profile constraints — it reuses the FP register file (no extra silicon),
> supports tail predication natively (no scalar cleanup loops), and is optimized for INT8/INT16 workloads
> typical in quantized ML. Neon is wider and faster in absolute terms, but Helium delivers the best
> ops-per-milliwatt for MCU-class devices.

---

### Multi-Layer Perceptron (MLP) — Simplest Neural Network

#### The Single Neuron (Perceptron)

A single neuron computes:

**z = Σ(wᵢ · xᵢ) + b**

Which expands to: z = (w₁·x₁) + (w₂·x₂) + (w₃·x₃) + ... + b

```
Inputs         Weights         Sum + Bias        Activation       Output

x₁ ──────── w₁ ──────┐
                      │
x₂ ──────── w₂ ──────┼──────► [ Σ(wi·xi) + b ] ──► f(z) ──────► y
                      │              z
x₃ ──────── w₃ ──────┘

                      b (bias)
```

**What each part means:**

| Symbol | What it is | Analogy |
|---|---|---|
| xᵢ | Inputs (your data — e.g., pixel values) | Raw ingredients |
| wᵢ | Weights (learned during training) | How important each input is |
| b | Bias (also learned) | A baseline offset |
| z | Weighted sum — the neuron's raw calculation | Recipe result before cooking |
| f(z) | Activation function (ReLU, sigmoid, etc.) | Decides if neuron "fires" |
| y | Output | Final answer from this neuron |

#### Concrete Example

```
x₁ = 200 (bright pixel),  x₂ = 50 (dark pixel),  x₃ = 150 (mid pixel)
w₁ = 0.3,  w₂ = -0.1,  w₃ = 0.5,  b = 2.0

z = (0.3 × 200) + (-0.1 × 50) + (0.5 × 150) + 2.0
z = 60 + (-5) + 75 + 2 = 132

After ReLU activation: y = max(0, 132) = 132
```

#### Multi-Layer Perceptron = Many Neurons Stacked

```
Input Layer      Hidden Layer 1     Hidden Layer 2      Output Layer
(your data)      (learned features) (higher features)   (prediction)

  x₁ ──────────►  n₁ ──────────────►  n₄ ─────────────►  y₁ (rock)
       \        /    \            /      \
  x₂ ───\────/────►  n₂ ───────/─────►  n₅ ─────────────►  y₂ (paper)
       \  \/  /        \      /          /
  x₃ ───\─/\─/──────►  n₃ ──/──────────/──────────────────►  y₃ (scissors)
          /  \
         every neuron connects to every neuron in next layer
         (= "fully connected" or "dense" layer)
```

Each arrow has its own weight wᵢ. **Training** = finding the right values for ALL weights so the output matches the correct class.

#### In TensorFlow (code):

```python
import tensorflow as tf

model = tf.keras.Sequential([
    tf.keras.layers.Flatten(input_shape=(320, 320, 3)),  # 307,200 inputs
    tf.keras.layers.Dense(128, activation='relu'),        # Hidden layer: 128 neurons
    tf.keras.layers.Dense(64, activation='relu'),         # Hidden layer: 64 neurons
    tf.keras.layers.Dense(3, activation='softmax')        # Output: 3 classes (rock/paper/scissors)
])
```

Each `Dense(128)` layer = 128 neurons, each computing z = Σ(wᵢ·xᵢ) + b then applying ReLU.

#### Why this relates to the project:

The YOLO model uses Conv2D instead of Dense layers, but at the **lowest level**, every operation in the neural network boils down to: **multiply, accumulate, add bias**. This is exactly what Helium's 16-wide INT8 MAC instructions accelerate — doing 16 of these wᵢ·xᵢ multiplications in a single CPU cycle.

---

### Basic CNN Architecture (Convolutional Neural Network)

A CNN is designed to process **images**. Unlike MLP (which flattens all pixels into one long list), CNN preserves the **spatial structure** (2D grid) of the image and detects patterns locally.

#### The 5 Core Layers

```
INPUT IMAGE (320×320×3 RGB)
    │
    ▼
┌─────────────────────────────────────────────────────────────────┐
│  Layer 1: CONVOLUTIONAL LAYER                                    │
│                                                                   │
│  A small filter (e.g., 3×3) slides across the image.             │
│  At each position, it does: Σ(filter × patch) + bias = 1 value  │
│                                                                   │
│  Input image         Filter (3×3)        Output (Feature Map)    │
│  ┌───────────┐       ┌─────┐             ┌───────────┐          │
│  │ . . . . . │       │ 1 0 │             │ . . . . . │          │
│  │ . █ █ . . │   *   │ 0 1 │     =       │ . . 5 . . │          │
│  │ . █ █ . . │       │ 1 0 │             │ . 3 . 2 . │          │
│  │ . . . . . │       └─────┘             │ . . . . . │          │
│  └───────────┘                           └───────────┘          │
│                                                                   │
│  Multiple filters → multiple feature maps (edges, corners, etc.) │
└───────────────────────────────┬─────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│  Layer 2: ACTIVATION (ReLU)                                      │
│                                                                   │
│  f(x) = max(0, x)                                                │
│                                                                   │
│  Negative values → 0,  Positive values → unchanged               │
│  Purpose: introduce non-linearity (without this, stacking layers │
│           would be no better than one layer)                      │
└───────────────────────────────┬─────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│  Layer 3: POOLING (Max Pooling)                                  │
│                                                                   │
│  Takes a 2×2 window, keeps only the MAX value                    │
│                                                                   │
│  ┌─────────┐         ┌─────┐                                    │
│  │ 1  3 │ 2  4 │     │ 3 │ 4 │                                  │
│  │ 5  2 │ 1  6 │ →   │ 5 │ 6 │    (halves the spatial size)    │
│  └─────────┘         └─────┘                                    │
│                                                                   │
│  Purpose: reduces size (less computation), keeps important info   │
│  320×320 → 160×160 → 80×80 → ... gets smaller each time         │
└───────────────────────────────┬─────────────────────────────────┘
                                │
                                ▼
            [Repeat Conv → ReLU → Pool several times]
            (each layer detects higher-level features:
             edges → textures → shapes → objects)
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│  Layer 4: FLATTEN + FULLY CONNECTED (Dense)                      │
│                                                                   │
│  Flatten the final feature maps into a 1D vector                 │
│  Then pass through Dense layers (like MLP):                      │
│                                                                   │
│  [10×10×64] → Flatten → [6400] → Dense(128) → Dense(64)         │
│                                                                   │
│  Purpose: learn which combination of features = which class      │
└───────────────────────────────┬─────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│  Layer 5: OUTPUT (Softmax)                                       │
│                                                                   │
│  Dense(3, activation='softmax')                                  │
│                                                                   │
│  Output: [0.05, 0.90, 0.05] = [rock, paper, scissors]           │
│           ↑                                                       │
│           "90% confident this is Paper"                           │
└─────────────────────────────────────────────────────────────────┘
```

#### What the Convolutional Filter Actually Does

```
Image patch (3×3)     Filter weights (3×3)       Result
┌─────────────┐       ┌─────────────┐
│ 10  20  30  │       │  1   0  -1  │
│ 40  50  60  │   ×   │  1   0  -1  │  =  Σ = (10×1)+(20×0)+(30×-1)
│ 70  80  90  │       │  1   0  -1  │       +(40×1)+(50×0)+(60×-1)
└─────────────┘       └─────────────┘       +(70×1)+(80×0)+(90×-1)
                                            = 10-30+40-60+70-90 = -60

This particular filter detects VERTICAL EDGES (left vs right difference)
```

The filter slides across the entire image producing a **feature map** — a heatmap of where that pattern (edge, corner, curve) appears.

#### CNN vs MLP — Why CNN is better for images

| | MLP (Dense) | CNN (Convolutional) |
|---|---|---|
| Connections | Every pixel → every neuron | Small filter → local patch |
| Parameters for 320×320×3 input | 307,200 × neurons = millions | 3×3×3 × num_filters = hundreds |
| Spatial awareness | None (flat vector) | Preserves 2D structure |
| Translation invariance | No (pixel moves = different input) | Yes (same filter detects pattern anywhere) |

#### How this maps to the project

The YOLO model on PSOC Edge is a CNN:
- **Conv layers** detect visual features (finger shapes, hand edges)
- **Pooling** reduces 320×320 down progressively
- **Final layers** output bounding boxes + class scores
- **Every Conv operation** = sliding a filter across the image = millions of MAC operations = exactly what Helium's 16-wide INT8 SIMD accelerates

---


