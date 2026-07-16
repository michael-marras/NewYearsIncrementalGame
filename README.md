<h1 align="center">NewYearsIncrementalGame</h1>

<p align="center">Embark on a delightful journey of passive progress and resource management, crafted as a foundational C++ game development project.</p>

---

## New Years Incremental Game Jam 2026
* This is our entry for the New Years Incremental Game Jam 2026
* It's also our first time working together and one of our very first personal coding projects
* There's alot of rough edges in this project when it comes to documentation, coding practices, and organization, hence why it was a very impactful learning experience
* Here's the playable demo/ our final game jam entry. Enjoy! [https://hzhu.itch.io/pixel-god]

## Technical Architecture

The project leverages a robust and efficient tech stack to deliver a responsive and engaging experience.

| Technology | Purpose | Key Benefit |
| :--------- | :------ | :---------- |
| C++        | Core Game Logic & Engine | High performance, low-level control, industry standard for games |
| SDL3       | Graphics, Audio, Input Handling | Cross-platform multimedia support, lightweight, direct control |
| Make/CMake | Build System Automation | Streamlined compilation, dependency management |

### Directory Structure

```
NewYearsIncrementalGame/
├── 📄 README.md
├── 📁 SDL/
│   └── ... (SDL library files and headers)
├── 📁 examples/
│   └── 📄 basic_usage.cpp
├── 📁 src/
│   ├── 📄 main.cpp
│   ├── 📄 game.hpp
│   ├── 📄 game.cpp
│   ├── 📄 player.hpp
│   ├── 📄 player.cpp
│   ├── 📄 resource.hpp
│   ├── 📄 resource.cpp
│   └── 📄 upgrade.hpp
│   └── 📄 upgrade.cpp
└── 📄 LICENSE.md
```

## Operational Setup

### Prerequisites

Before you begin, ensure you have the following installed on your system:

*   A C++ compiler (e.g., GCC, Clang, MSVC)
*   `make` (or `cmake` if using a CMake build system)
*   SDL3 Development Libraries (runtime and development headers appropriate for your OS)

### Installation

Follow these steps to get your local copy of `NewYearsIncrementalGame` up and running:

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/NewYearsIncrementalGame.git
    cd NewYearsIncrementalGame
    ```

2.  **Build the project:**
    Assuming a `Makefile` is present, compile the game:
    ```bash
    make
    ```
    *If using CMake, the steps would typically be:*
    ```bash
    mkdir build && cd build
    cmake ..
    make
    ```

3.  **Run the game:**
    ```bash
    ./NewYearsIncrementalGame
    ```
    *(If built with CMake, navigate to the `build` directory and run the executable from there.)*

## Community & Governance

### Contributing

1.  **Create a new branch** for your feature or bug fix:
    ```bash
    git checkout -b feature/YourFeatureName
    ```
2.  **Implement your changes** and ensure they align with the project's goals and coding standards.
3.  **Commit your changes** with a clear and concise message:
    ```bash
    git commit -m 'feat: Add Your Feature'
    ```
4.  **Push to the branch** on your forked repository:
    ```bash
    git push origin feature/YourFeatureName
    ```
5.  **Open a Pull Request** against the `main` branch of this repository, providing a detailed description of your changes and their benefits.
