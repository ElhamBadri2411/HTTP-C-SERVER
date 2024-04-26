import requests
import threading
import time

# *** Customize these ***
URL = "http://localhost:8080/json?id=111"  # Replace with your server's URL
NUM_REQUESTS = 500  # Number of requests to send
NUM_THREADS = 10  # Number of simultaneous threads

failed_requests = 0


def send_request():
    global failed_requests
    try:
        response = requests.get(URL)
        response.raise_for_status()  # Raise an exception for error status codes
        print("Request successful")
    except requests.exceptions.RequestException as e:
        print(f"Request failed: {e}")
        failed_requests += 1


start_time = time.time()

# Create and start threads
threads = []
for _ in range(NUM_THREADS):
    for _ in range(NUM_REQUESTS // NUM_THREADS):  # Distribute requests among threads
        t = threading.Thread(target=send_request)
        t.start()
        threads.append(t)

# Wait for all threads to complete
for t in threads:
    t.join()

end_time = time.time()
total_time = end_time - start_time

print(f"\n--- Test Results ---")
print(f"Total requests: {NUM_REQUESTS}")
print(f"Failed requests: {failed_requests}")  # Display failures
print(f"Time taken: {total_time:.2f} seconds")
