# gRPC Template with Support for CUDA and Python Deployment
# For Development in C++:
```bash
# To Test:
docker-compose up --build test 

# To Run:
docker-compose up --build app
```

# To Run Nginx as a Server:
1. SSL and TLS Configuration
```bash
sudo apt-get install certbot python3-cerbot-nginx
sudo certbot --nginx -d yourdomain.com
```

2. Open Firewall Ports
```bash
sudo ufw allow 443/tcp
sudo systemctl restart nginx
```

# To use Dart as Client
1. Grab Proto Files and Compile them using Dart
2. Create a stub
```dart
final channel = ClientChannel(
    'mindbridge-study.com', // Your domain name
    port: 443, // Assuming you're using HTTPS with the standard port
    options: const ChannelOptions(
        credentials: ChannelCredentials.secure(), // For HTTPS
    ),
);

final stub = RouteGuideClient(channel);

try {
    // Example: calling a simple RPC
    final response = await stub.uploadImage(Stream.value(ImageChunk(data: [YOUR DATA HERE])));
    print('UploadImage responded: ${response.message}');
} catch (e) {
    print('Caught error: $e');
}

await channel.shutdown();

```

# @TODO!:
1. Get a Domain and add SSL and TLS Certifications
2. Add Firebase Authentication
3. Test on an Ubuntu System
4. Maybe: Shift Tasks onto GPU for more distributed processing if no image processing is done
