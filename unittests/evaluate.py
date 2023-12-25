import unittest

from src import evaluate


class TestEvaluate(unittest.TestCase):
    def test_all_eval(self):
        self.assertEqual(evaluate.safe_eval('1 + 5'), 6)
        self.assertEqual(evaluate.safe_eval("'a' + 'b'"), 'ab')
        self.assertEqual(evaluate.safe_eval("a", {'a': 'b'}), 'b')
        self.assertEqual(evaluate.safe_eval("a + b", vars={'a': 1, 'b': 4}), 5)
        self.assertTrue(evaluate.safe_eval("'a' == 'a'"))
        self.assertTrue(evaluate.safe_eval("'a' != 'b'"))
        self.assertFalse(evaluate.safe_eval("4 <= x < 10", vars={'x' : 11}))
        self.assertTrue(evaluate.safe_eval("'a' == x", vars={'x' : 'a'}))
        self.assertFalse(evaluate.safe_eval("test=='hello'", vars={'test': 'ello'}))

if __name__ == 'main':
    unittest.main()
