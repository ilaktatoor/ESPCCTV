# Variables
PYTHON := python3
PIP := pip

# Default target
.PHONY: run
run: install
	$(PYTHON) main.py

# Install dependencies
.PHONY: install
install:
	$(PIP) install -r requirements.txt

# Clean installed packages (optional)
.PHONY: clean
clean:
	$(PIP) uninstall -y -r requirements.txt

# Generate requirements file (optional)
.PHONY: freeze
freeze:
	$(PIP) freeze > requirements.txt

