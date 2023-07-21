import temple
import unittest


class TestTempleParser(unittest.TestCase):
    def test_parser(self):
        """Test html template file containing all template actions."""
        output = None
        with open('./output.html', 'r') as f:
            output = f.read()

        self.assertEqual(temple.parse('./input.html'), output)


if __name__ == 'main':
    unittest.main()

