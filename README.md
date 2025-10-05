<p align="center">
<br>
  <img src="src/assets/fsr_image.gif" alt="Upscaling logo" width="400">
</p>

<h3 align="center">Real-time 3D Upscaling Demo</h3>

<p align="center">
  Real-time upscaling demo using C++ and OpenGL
  <br>
  <br>
</p>

![Static Badge](https://img.shields.io/badge/school_project-brown)
![Static Badge](https://img.shields.io/badge/langage-C++-blue)
![Static Badge](https://img.shields.io/badge/API-OpenGL-blue)
![Static Badge](https://img.shields.io/badge/tool-CMake-green)

This project is a simple **real-time upscaling demo** using C++ and OpenGL.  
It implements classic spatial upscalers and AMD-style techniques:

- **Nearest Neighbor**
- **Bilinear**
- **EASU** (Edge-Adaptive Spatial Upscaling)
- **RCAS** (Robust Contrast-Adaptive Sharpening)

The project includes a small demo app (textured cube + ImGui controls).

## Table of contents

- [Features](#-features)
- [Folder Structure](#-folder-structure)
- [Build Instructions](#-build-instructions)
- [Controls](#-controls)
- [Library Usage](#-library-usage)
- [License](#-license)

---

## ✨ Features
- Render at a lower resolution and upscale to your display.
- Switch between upscaling modes at runtime (via ImGui).
- Adjustable sharpening strength (for RCAS).

---

## 📂 Folder Structure

```
upscaler-demo/
├── dependencies/                   # Project dependencies (e.g., third-party libraries)
│   ├── include/                       
│   └── lib/               
├── public/                         # Static assets like images                       
│   └── images                      
├── src/
│   ├── shaders/                    # GLSL shader files
│   └── main.cpp                    # Main application entry point
└── CMakeLists.txt                  # CMake build script
```

---

## 🔧 Build Instructions

Requirements:
- CMake ≥ 3.29
- OpenGL 3.3+
- A C++23 compiler (GCC/Clang/MSVC)

Steps:

```bash
git clone https://github.com/Yojda/upscaler-demo.git
cd upscaler-demo
mkdir build && cd build
cmake ..
make
```

Run the demo:
```bash
./upscaler-demo
```

---

## 📜 License

MIT License – free to use and modify.