# Instructions to Run the Python Application

## Prerequisites

Before running the application, ensure you have the following installed on your system:

1. Python 3.x
2. pip (Python package installer)
3. Make utility (optional, but recommended for ease of use)

## Steps to Run the Application

### 1. Clone or Download the Repository
Ensure you have the project directory that contains the following files:
- `main.py` (The main Python script)
- `Makefile` (For automating tasks)
- `requirements.txt` (List of dependencies)

### 2. Install Dependencies
Run the following command to install the required Python dependencies:

```bash
make install
```

This will:
- Read the `requirements.txt` file.
- Install the necessary Python packages, including Flask, OpenCV, and NumPy.

If `make` is not installed, you can manually run:

```bash
pip install -r requirements.txt
```

### 3. Run the Application
Execute the following command to run the application:

```bash
make run
```

This will:
- Install the dependencies (if not already installed).
- Execute the `main.py` script using Python.

If `make` is not installed, you can manually run:

```bash
python3 main.py
```

### 4. Optional Commands

#### Freeze Installed Dependencies
To update the `requirements.txt` file with the current installed dependencies:

```bash
make freeze
```

#### Clean Installed Dependencies
To uninstall all the dependencies listed in `requirements.txt`:

```bash
make clean
```

## Troubleshooting

- **Missing `make` utility**: Install it using your system’s package manager. For example:
  - On Debian/Ubuntu:
    ```bash
    sudo apt install make
    ```
  - On Fedora:
    ```bash
    sudo dnf install make
    ```

- **Permission issues**: Use `sudo` if needed (e.g., `sudo make install`).

- **Missing Python or pip**: Ensure Python 3.x and pip are installed and properly configured in your system’s PATH.

## Additional Notes

- Ensure you’re in the correct directory (where `main.py`, `Makefile`, and `requirements.txt` are located) before executing any commands.
- If you’re using a virtual environment, activate it before running the `make` or `pip` commands:

  ```bash
  source <venv_directory>/bin/activate
  ```

# CCTV2 Flask Application Deployment Guide

## 1. Ensure All Files Are in Place

Your project directory should have the following files:

CCTV2/
    ├── cctv/
    │   ├── camara_pins.h
    │   └── cctv.ino
    ├── Dockerfile
    ├── main.py
    ├── Makefile
    ├── README.MD
    └── requirements.txt

## 2. Build the Docker Image

Run the following command in the terminal from the CCTV2 directory:

docker build -t flask-cctv .

- -t flask-cctv: Tags the image with the name flask-cctv.
- .: Specifies the current directory as the build context.

## 3. Run the Docker Container

Run the container using the docker run command:

docker run -d -p 5050:5050 --name flask-cctv-container flask-cctv

- -d: Runs the container in detached mode (in the background).
- -p 5050:5050: Maps port 5050 on the host to port 5050 in the container.
- --name flask-cctv-container: Names the container flask-cctv-container.
- flask-cctv: The name of the image built in Step 2.

## 4. Access Your Application

Open your browser and navigate to:

http://localhost:5050/video

This should serve your MJPEG stream.

## 5. Check Logs (Optional)

To view the logs of your running container, use:

docker logs flask-cctv-container

## 6. Stop the Container

If you want to stop the container, run:

docker stop flask-cctv-container

## 7. Restart the Container

To restart the container, run:

docker start flask-cctv-container

## 8. Remove the Container

To remove the container entirely:

docker rm -f flask-cctv-container

---

## Troubleshooting

- **Missing `make` utility**: Install it using your system’s package manager. For example:
    - On Debian/Ubuntu:
    sudo apt install make
    - On Fedora:
    sudo dnf install make

- **Permission issues**: Use `sudo` if needed (e.g., `sudo make install`).

- **Missing Python or pip**: Ensure Python 3.x and pip are installed and properly configured in your system’s PATH.

---

## Additional Notes

- Ensure you’re in the correct directory (where `main.py`, `Makefile`, and `requirements.txt` are located) before executing any commands.
- If you’re using a virtual environment, activate it before running the `make` or `pip` commands:

source <venv_directory>/bin/activate

---

## To update the `requirements.txt` file with the current installed dependencies:

make freeze

---

## Clean Installed Dependencies

To uninstall all the dependencies listed in `requirements.txt`:

make clean

--- 

Now your `README.md` includes both Docker deployment instructions and troubleshooting tips for a seamless experience.

