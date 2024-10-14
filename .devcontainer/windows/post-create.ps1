# post-create.ps1

# Exit immediately if a command exits with a non-zero status
$ErrorActionPreference = "Stop"

# Navigate to vcpkg directory
Set-Location C:/vcpkg

# Bootstrap vcpkg (already done in Dockerfile, but ensure it's up-to-date)
Write-Host "Bootstrapping vcpkg..."
.\bootstrap-vcpkg.bat

# Integrate vcpkg with VS Code
Write-Host "Integrating vcpkg with the environment..."
.\vcpkg integrate install

# Verify vcpkg installation
Write-Host "Verifying vcpkg installation..."
.\vcpkg.exe list

# Test Get-Clipboard functionality
Write-Host "Testing Get-Clipboard command..."
$clipboardContent = Get-Clipboard
Write-Host "Clipboard Content: $clipboardContent"

Write-Host "Post-create script completed successfully."
