# Temple
A dumb template-engine built for *extremely* simple usecases.

Supports:
  - Extension using base templates
  - Basic for-loops
  - HTML block insertion

### Using
Syntax is similar to many of the popular template-engines.

- Template actions must be on their own lines for now
- Extending a parent must include a block with the same name as parent's extension block (see `unittests/*.html` files for examples)

```html
{% extends "base.html" %}
<html>
  <body>
    {% block child %}
    {% endblock %}
  <body>
<html>
{% endblock %}
```

### Building HTML
Simply run `temple.build` command from the directory you want to compile.
`temple` can be installed as a local package `pip install -e .` from the root
directory of the project or the function can be imported directly with iPython
if needed.

```python
from temple import build

build('**/*.html', output_path='/var/www/html/cv')
```

### Testing
Package must be installed locally to run unit-tests `pip install -e .` from project root dir.
