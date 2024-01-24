import requests
import unittest
import subprocess

class MyWebServerTestCase(unittest.TestCase):
    def setUp(self):
        # Start your web server or setup a testing environment
        self.server_process = subprocess.Popen(['./webserv'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    def tearDown(self):
        # Stop your web server or clean up resources
        self.server_process.terminate()
        self.server_process.wait()

    def test_get_request(self):
        # Make a GET request to your server
        response = requests.get('http://localhost:8080')

        # Check the response status code and content
        self.assertEqual(response.status_code, 200)
        self.assertIn('Welcome to Webserv!', response.text)

    # def test_post_request(self):
    #     # Make a POST request to your server with data
    #     data = {'key': 'value'}
    #     response = requests.post('http://localhost:8080', data=data)

    #     # Check the response status code and content
    #     self.assertEqual(response.status_code, 201)
    #     self.assertIn('Created', response.text)

if __name__ == '__main__':
    unittest.main()
