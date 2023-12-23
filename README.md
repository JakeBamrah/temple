# Temple
A simple template-engine built for *extremely* simple usecases.

Supports:
  - HTML block insertion
  - Expressions (conditionals and variables)
  - Template inheritance (extension) using base templates
  - Recursive inheritance support (one parent / extension per template doc)

### Usage
Syntax is similar to many of the popular template-engines.

`{{ ... }}` - Expressions, extending and variables

`{% ... %}`  - Block statements such as `if` / `else`

**Note:** Conditional logic can only do truthy checks and cannot evaluate comparisons as of yet.

#### Variables
```html
<html>
  <body>
    <div>
      This is a var: {{ expr_variable }}
    </div>
  </body>
<html>
```

#### Conditionals
```html
<html>
  <body>
    {% if boolean_value %}
      <div>Truthy conditional</div>
    {% else %}
      <div>Falsey conditional</div>
    {% endblock %}
  </body>
<html>
```

#### Inheritance
```
<!-- parent template -->
<html>
  <head>
    {% insert head %}{% endblock %}
  </head>
  <body>
    {% insert child %}{% endblock %}
  </body>
<html>
```

```html
<!-- child template -->
{{ extends base.html }}
{% insert head %}
  <style>
    p {
      color: red;
    }
  </style
{% endblock %}
{% insert child %}
  <div>
    This is a child
  </div>
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
