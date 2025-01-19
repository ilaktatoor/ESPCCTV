# Use an official Python runtime as the base image
FROM python:3.10-slim

# Set the working directory inside the container
WORKDIR /app

# Copy the Python script and configuration files into the container
COPY main.py requirements.txt Makefile README.MD /app/

# Copy the cctv directory
COPY cctv /app/cctv

# Install Python dependencies
RUN pip install --no-cache-dir -r requirements.txt

# Expose any necessary ports (if your application serves via Flask or other web frameworks, e.g., port 5050)
EXPOSE 5050

# Command to execute your Python script
CMD ["python", "main.py"]

