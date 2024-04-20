from setuptools import setup
from pathlib import Path

package_data = {
    'grpcnvidia': [
        'build/gRPCNvidia.so'
    ],
}

setup(
    name="gRPCNvidia",
    version="0.0.1",
    author="Omar",
    author_email="ok.elsawy@gmail.com",
    description="Nvidia gRPC module for hosting servers/clients using gRPC for Python 3.6+",
    long_description=(Path(__file__).parent / "README.md").read_text(encoding="utf-8"),
    long_description_content_type="text/markdown",
    url="https://github.com/OmarSiwy/gRPCNvidia",
    packages=['gRPCNvidia'],  # Replace with your actual Python package directory
    package_data=package_data,
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
)
