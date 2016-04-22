# mecab.el

[libmecab](http://mecab.googlecode.com/svn/trunk/mecab/doc/index.html) binding of Emacs Lisp.

## Interfaces

#### `(mecab-new &rest dictionaries)`

Initialize mecab instance and return it.

#### `(mecab-sparse-to-string mecab input &optional limit)`

Parse `input` string and return result as string.

#### `(mecab-sparse-to-list mecab input &optional limit)`

Parse `input` string and return result as list of string.
