import requests
from colorama import init, Fore, Style

# Define the base URL for the web server
BASE_URL = 'http://localhost:8080'

# Function to send GET requests for static files
def test_static_files():
    print(Fore.YELLOW + f"Testing GET static files...\n")
    files = ['/index.html', '/public/image/cat/1.png', '/nonexistent.html']
    for file in files:
        response = requests.get(BASE_URL + file)
        expected_status = 200 if file != '/nonexistent.html' else 404
        print(Fore.MAGENTA + f"Static GET request for '{file}': Expected {expected_status}, Actual {response.status_code}")
        if response.status_code == expected_status:
            print(Fore.GREEN + f"[SUCCESS]")
        else:
            print(Fore.RED + "[FAILURE]")

# Function to send GET requests to non-existing URLs
def test_non_existing_urls():
    print(Fore.YELLOW + f"Testing GET non-existing directories...\n")
    urls = ['/nonexistent', '/invalid', '/404error']
    for url in urls:
        response = requests.get(BASE_URL + url)
        print(Fore.MAGENTA + f"GET request to '{url}': Expected 404, Actual {response.status_code}")
        if response.status_code == 404:
            print(Fore.GREEN + f"[SUCCESS]")
        else:
            print(Fore.RED + "[FAILURE]")

# Functoin to send GET request to autoindex directory
def test_dir():
    print(Fore.YELLOW + f"Testing GET to autoindex directories...\n")
    url = "/public/"
    response = requests.get(BASE_URL + url)
    print(Fore.MAGENTA + f"GET request to '{url}': Expected 200, Actual {response.status_code}")
    if response.status_code == 200:
        print(Fore.GREEN + f"[SUCCESS]")
    else:
        print(Fore.RED + "[FAILURE]")

# Function to send POST requests to the upload endpoint with large files
def test_large_file_upload():
    # Create a large file (greater than client_max_body_size)
    large_file_path = 'large_file.txt'
    with open(large_file_path, 'wb') as file:
        file.write(b'0' * 25 * 1024 * 1024)  # 25 MB file
    
    # Send a POST request with the large file
    files = {'file': open(large_file_path, 'rb')}
    response = requests.post(BASE_URL + '/upload/upload.py', files=files)
    
    print(Fore.MAGENTA + f"Large file upload: Expected 413, Actual", response.status_code)
    if response.status_code == 413:
        print(Fore.GREEN + f"[SUCCESS]")
    else:
        print(Fore.RED + "[FAILURE]")

# Function to make a DELETE request and print the result
def test_delete(endpoint):
    url = BASE_URL + endpoint
    response = requests.delete(url)
    print(Fore.MAGENTA + f"DELETE request on file: {endpoint}: Expected 200, Actual {response.status_code}")
    if response.status_code == 200:
        print(Fore.GREEN + f"[SUCCESS]")
    else:
        print(Fore.RED + "[FAILURE]")

# Edge cases
def test_404_edge_case():
    print(Fore.YELLOW + f"Testing 404 edge cases...\n")
    urls = ['/index.html/']
    for url in urls:
        response = requests.get(BASE_URL + url)
        print(Fore.MAGENTA + f"GET request to '{url}': Expected 404, Actual {response.status_code}")
        if response.status_code == 404:
            print(Fore.GREEN + f"[SUCCESS]")
        else:
            print(Fore.RED + "[FAILURE]")

# Function to execute all test cases
def run_tests():
    test_static_files()
    print("")
    test_non_existing_urls()
    print("")
    test_dir()
    print("")
    test_large_file_upload()
    print("")
    test_delete("/delete/be-deleted.txt")
    # print("")
    # test_404_edge_case()
    print(Fore.YELLOW + f"\nAll test cases executed.")

# Entry point of the script
if __name__ == "__main__":
    run_tests()
