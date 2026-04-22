# Block Blast Clone with C and Raylib

## Project Overview
This project is a clone of the Block Blast game developed using C and the Raylib library. The main goal is to build a modular structure by combining low-level skills like memory management and pointer manipulation with game development dynamics. The use of external libraries is prohibited.

## Setup and Compilation
This project uses **CMake** and **Raylib**. The Raylib dependency is automatically downloaded from GitHub via CMake (`FetchContent`).

### Prerequisites
To compile and run the project, the following tools must be installed:
* **Git:** To download Raylib in the background ([Download](https://git-scm.com/downloads)).
* **C/C++ Compiler:** 
  * **Windows:** [Visual Studio C++ Build Tools (MSVC)](https://visualstudio.microsoft.com/visual-cpp-build-tools/) or [MinGW/GCC (MSYS2)](https://www.msys2.org/).
  * **macOS:** Xcode Command Line Tools (via `xcode-select --install`).
  * **Linux:** GCC or Clang (via `sudo apt install build-essential`).
* **CMake:** To manage the build process ([Download](https://cmake.org/download/)).

### Running with VS Code
1. Ensure Git, CMake, and a suitable compiler are installed.
2. Open this project with VS Code.
3. Install the **C/C++** and **CMake Tools** extensions.
4. Select the correct **Compiler Kit** from the bottom status bar or command palette (`CMake: Select a Kit`).
5. Click **Build**, and once successful, click **Play** to start the game.

## Technical Constraints & Architecture
* **Data Structures:** Game elements defined using structs.
* **Memory Management:** Dynamic memory control with `malloc`/`free`.
* **Algorithms:** Multi-dimensional arrays, loops, and conditional statements.
* **Pointers:** Data transfer between functions and memory manipulation.
* **File I/O:** Saving high scores and game state.
* **Animation:** Custom timer logic using raw code and Raylib's `GetFrameTime()`.

## Game Logic
* **Collision Detection:** Pointer-based overlap testing between the dragged matrix and the target grid.
* **Grid Clearing & Combos:** Row/column scanning. Score = Base × N × Combo (N: number of simultaneously cleared lines).
* **Game Over:** Background simulation checking if available blocks fit into empty spaces.

## Delivery and Evaluation
Deliverables: Source code, project report, a maximum 5-minute demo video, and a live presentation. Every team member must be familiar with the entire codebase.

| Criterion | Weight |
| --- | --- |
| Use of Mandatory Programming Concepts | 25% |
| Oral Presentation & Jury Performance | 25% |
| Code Quality & Correctness | 20% |
| Creativity & Game Design | 15% |
| Report & Documentation | 10% |
| Video Presentation | 5% |


Ayarlar buton haline getirilecek
Ses efekti ve arka plan müziği eklenecek
Adventure mode eklenecek