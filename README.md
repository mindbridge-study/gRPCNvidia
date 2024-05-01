# gRPC Template with Support for CUDA and Python Deployment

# For Development in C++:

```bash
# To Test/Run:
docker-compose up --build
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

1. Get a Domain and add SSL and TLS Certifications for serving (Peterson)
2. Test with images
3. Add Firebase Authentication
4. Send and recieve images from gemini (waiting on roy to give me keys)
5. Replace new with cuda calls
