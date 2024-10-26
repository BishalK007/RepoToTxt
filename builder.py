import os
import subprocess
import json
import re
import shutil
import argparse
import zipfile
import sys

# Configuration variables
USE_VCPKG = "ON"
BUILD_DIR = "build-win"
PROJECT_TEMP_OVERRIDDEN = "0"

# Function to log information
def log_info(message):
    print(f"\033[32m[INFO]\033[0m {message}")

# Function to log errors
def log_error(message):
    print(f"\033[31m[ERROR]\033[0m {message}", file=sys.stderr)

# Function to sanitize the project name
def sanitize_package_name(name):
    name = re.sub(r'([a-z0-9])([A-Z])', r'\1-\2', name).lower()  # Replace uppercase-lowercase boundaries with hyphens
    name = re.sub(r'[^a-z0-9-]', '-', name)  # Replace non-alphanumeric characters with hyphens
    name = re.sub(r'-{2,}', '-', name)  # Replace multiple hyphens with a single hyphen
    name = name.strip('-')  # Remove leading or trailing hyphens
    return name

# Function to read metadata from META.json
def read_meta():
    if not os.path.exists("META.json"):
        log_error("META.json file not found.")
        sys.exit(1)

    with open("META.json", "r") as f:
        meta = json.load(f)
        
    global PROJECT_NAME, EXECUTABLE_NAME, DESCRIPTION, MAINTAINER_NAME, MAINTAINER_EMAIL, GITHUB_USERNAME, GITHUB_REPO_URL, PROJECT_VERSION
    PROJECT_NAME = sanitize_package_name(meta.get("project_name", ""))
    EXECUTABLE_NAME = meta.get("executable_name", "")
    DESCRIPTION = meta.get("description", "")
    MAINTAINER_NAME = meta["maintainer"].get("name", "")
    MAINTAINER_EMAIL = meta["maintainer"].get("email", "")
    GITHUB_USERNAME = meta["maintainer"].get("githubUsername", "")
    GITHUB_REPO_URL = meta.get("githubRepoUrl", "")
    
    if not os.path.exists("VERSION"):
        log_error("VERSION file not found.")
        sys.exit(1)
    
    with open("VERSION", "r") as f:
        PROJECT_VERSION = f.read().strip()

    if not PROJECT_NAME:
        log_error("Sanitized PROJECT_NAME is empty. Please check META.json.")
        sys.exit(1)

    log_info(f"Sanitized Project Name: {PROJECT_NAME}")

# Function to configure the CMake project
def configure_project():
    log_info("Configuring CMake project...")
    
    if PROJECT_TEMP_OVERRIDDEN == "0":
        generate_vcpkg_json()

    cmake_command = [
        "cmake", "-B", BUILD_DIR, "-S", ".", 
        f"-DUSE_VCPKG={USE_VCPKG}", 
        f"-DPROJECT_NAME={PROJECT_NAME}", 
        f"-DEXECUTABLE_NAME={EXECUTABLE_NAME}", 
        f"-DPROJECT_VERSION={PROJECT_VERSION}",
        f"-DDESCRIPTION={DESCRIPTION}",  # Add this line
        f"-DMAINTAINER_NAME={MAINTAINER_NAME}",  # Add if needed
        f"-DMAINTAINER_MAIL={MAINTAINER_EMAIL}",  # Add if needed
        f"-DHOMEPAGE={GITHUB_REPO_URL}"  # Add if needed
    ]
    subprocess.run(cmake_command, check=True)

    log_info("CMake configuration complete.")

# Function to build the project
def build_project(build_type="Debug"):
    log_info(f"Building project in {build_type} mode...")

    if not os.path.isdir(BUILD_DIR):
        log_info("Build directory not found. Configuring project...")
        configure_project()
    
    build_command = ["cmake", "--build", BUILD_DIR, "--config", build_type]
    subprocess.run(build_command, check=True)

    if build_type == "Release":
        cpack_command = ["cpack", "-G", "NSIS"]
    subprocess.run(cpack_command, cwd=BUILD_DIR, check=True)

    log_info("Build complete.")

# Function to run the executable
def run_executable():
    log_info("Running the executable...")

    executable_path = os.path.join(BUILD_DIR, EXECUTABLE_NAME)
    if os.path.isfile(executable_path):
        log_info(f"Executing {executable_path}")
        subprocess.run([executable_path], check=True)
    else:
        log_error(f"Executable {EXECUTABLE_NAME} not found in {BUILD_DIR}. Please build the project first.")

# Function to zip files
def zip_files():
    log_info("Zipping files...")

    with zipfile.ZipFile("files.zip", "w", zipfile.ZIP_DEFLATED) as zipf:
        for root, dirs, files in os.walk("."):
            if "files.zip" in files:
                files.remove("files.zip")
            if root.startswith("./" + BUILD_DIR) or root.startswith("./."):
                continue
            for file in files:
                file_path = os.path.join(root, file)
                zipf.write(file_path)

    log_info("Zipping complete. Created files.zip.")

import os
import shutil

# Function to generate vcpkg.json using metadata
def generate_vcpkg_json():
    log_info("Generating vcpkg.json from META.json and VERSION...")
    
    # Define file paths
    original_file = "vcpkg.json"
    template_file = "vcpkg.json.template"
    output_file = original_file
    
    # Copy vcpkg.json to vcpkg.json.template
    if os.path.exists(original_file):
        shutil.copyfile(original_file, template_file)
    else:
        log_error(f"{original_file} not found.")
        sys.exit(1)
    
    # Check that template file now exists
    if not os.path.exists(template_file):
        log_error(f"{template_file} not found after copying.")
        sys.exit(1)
    
    # Replace placeholder values and write to output_file
    with open(template_file, "r") as template, open(output_file, "w") as output:
        for line in template:
            line = line.replace('"name": "project-name"', f'"name": "{PROJECT_NAME}"')
            line = line.replace('"version-string": "0.0.0"', f'"version-string": "{PROJECT_VERSION}"')
            output.write(line)
    
    # Delete vcpkg.json.template after generation
    os.remove(template_file)
    
    log_info(f"{output_file} has been updated and {template_file} has been deleted.")

# Command-line argument parsing
def parse_args():
    parser = argparse.ArgumentParser(description="Windows Build Automation Script")
    parser.add_argument("--conf", action="store_true", help="Configure the CMake project")
    parser.add_argument("--build", action="store_true", help="Build the project")
    parser.add_argument("--run", action="store_true", help="Run the executable after building")
    parser.add_argument("--zip", action="store_true", help="Zip project files")
    parser.add_argument("--exe", type=str, help="Specify the name of the executable")
    parser.add_argument("--proj", type=str, help="Specify the project name")
    parser.add_argument("--ver", type=str, help="Specify the project version")
    parser.add_argument("--create-exe", action="store_true", help="Build the project in Release mode and create a .exe installer")
    args = parser.parse_args()
    return args

# Main execution starts here
if __name__ == "__main__":
    args = parse_args()

    read_meta()
    
    if args.exe:
        EXECUTABLE_NAME = args.exe
        PROJECT_TEMP_OVERRIDDEN = "1"
    
    if args.proj:
        PROJECT_NAME = sanitize_package_name(args.proj)
        PROJECT_TEMP_OVERRIDDEN = "1"
    
    if args.ver:
        PROJECT_VERSION = args.ver
        PROJECT_TEMP_OVERRIDDEN = "1"

    if args.conf:
        configure_project()
    
    if args.build:
        build_project()
    
    if args.run:
        run_executable()
    
    if args.zip:
        zip_files()
    
    if args.create_exe:
        # Build in release mode to create the .exe file
        build_project("Release")
