# gRPC Template with Support for CUDA and Python Deployment
# For Development in C++:
```bash
# To Test:
docker-compose up --build test 

# To Run:
docker-compose up --build app
```

# For Uploading to Python:
```bash
# Test if Extension Works
docker-compose up --build extension

# Upload to Your Github (Make sure to change setup.py and setup.cfg for different name, as well as folder name (gRPCNvidia) to your package name)
git add .
git commit -m ""
git push

# Store pypi_password in your github env

# Install Extension
pip install YOUR_EXTENSION_NAME
```

# Used in the Following Projects:
- [gRPCNvidia](https://github.com/mindbridge-study/gRPCNvidia)
