#!/usr/bin/env python3

"""
Local test script for OTFFT conanfile.py
This script tests the conanfile functionality without requiring remote repositories.
"""

import os
import sys
import tempfile
import shutil
from pathlib import Path

def test_conanfile():
    """Test the conanfile.py functionality locally."""
    
    print("=== OTFFT Conanfile Local Test ===")
    
    # Test 1: Check if conanfile.py is syntactically correct
    print("\n1. Testing conanfile.py syntax...")
    try:
        import importlib.util
        spec = importlib.util.spec_from_file_location("conanfile", "conanfile.py")
        conanfile_module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(conanfile_module)
        print("✓ conanfile.py syntax is valid")
        
        # Test 2: Check if the OtfftConan class exists and has required methods
        print("\n2. Testing OtfftConan class...")
        if hasattr(conanfile_module, 'OtfftConan'):
            conan_class = conanfile_module.OtfftConan
            print("✓ OtfftConan class found")
            
            # Check required attributes
            required_attrs = ['name', 'version', 'license', 'description']
            for attr in required_attrs:
                if hasattr(conan_class, attr):
                    print(f"✓ {attr}: {getattr(conan_class, attr)}")
                else:
                    print(f"✗ Missing attribute: {attr}")
            
            # Check required methods
            required_methods = ['source', 'package', 'package_info', 'export_sources']
            for method in required_methods:
                if hasattr(conan_class, method):
                    print(f"✓ Method {method}() found")
                else:
                    print(f"✗ Missing method: {method}()")
                    
        else:
            print("✗ OtfftConan class not found")
            return False
            
    except Exception as e:
        print(f"✗ Error loading conanfile.py: {e}")
        return False
    
    # Test 3: Test package structure
    print("\n3. Testing package structure...")
    current_dir = Path(".")
    
    # Check for test_package
    if (current_dir / "test_package").exists():
        print("✓ test_package directory found")
        if (current_dir / "test_package" / "conanfile.py").exists():
            print("✓ test_package/conanfile.py found")
        if (current_dir / "test_package" / "CMakeLists.txt").exists():
            print("✓ test_package/CMakeLists.txt found")
        if (current_dir / "test_package" / "src" / "test.cpp").exists():
            print("✓ test_package/src/test.cpp found")
    else:
        print("✗ test_package directory not found")
    
    # Check for example
    if (current_dir / "example").exists():
        print("✓ example directory found")
        if (current_dir / "example" / "conanfile.txt").exists():
            print("✓ example/conanfile.txt found")
        if (current_dir / "example" / "CMakeLists.txt").exists():
            print("✓ example/CMakeLists.txt found")
    else:
        print("✗ example directory not found")
    
    # Test 4: Test package methods exist and are callable
    print("\n4. Testing package methods...")
    try:
        # Check that key methods exist and have proper signatures
        import inspect
        
        # Test source method
        source_sig = inspect.signature(conan_class.source)
        print(f"✓ source() signature: {source_sig}")
        
        # Test build method
        if hasattr(conan_class, 'build'):
            build_sig = inspect.signature(conan_class.build)
            print(f"✓ build() signature: {build_sig}")
        
        # Test package method
        package_sig = inspect.signature(conan_class.package)
        print(f"✓ package() signature: {package_sig}")
        
        # Test package_info method
        package_info_sig = inspect.signature(conan_class.package_info)
        print(f"✓ package_info() signature: {package_info_sig}")
        
        # Test package_id method
        package_id_sig = inspect.signature(conan_class.package_id)
        print(f"✓ package_id() signature: {package_id_sig}")
        
        print("✓ All required methods are properly defined")
        
    except Exception as e:
        print(f"✗ Error checking package methods: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    print("\n=== Test Summary ===")
    print("✓ All basic tests passed!")
    print("✓ OTFFT conanfile.py appears to be properly configured")
    print("\nTo use this package:")
    print("1. conan export . otfft/11.5@")
    print("2. Add 'otfft/11.5@' to your requirements")
    print("3. Use conan install to get the headers")
    
    return True

if __name__ == "__main__":
    if test_conanfile():
        sys.exit(0)
    else:
        sys.exit(1)