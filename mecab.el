;;; mecab.el --- mecab binding of Emacs Lisp

;; Copyright (C) 2016 by Syohei YOSHIDA

;; Author: Syohei YOSHIDA <syohex@gmail.com>
;; URL: https://github.com/syohex/emacs-mecab
;; Version: 0.01

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:

;;; Code:

(require 'cl-lib)
(require 'mecab-core)

;;;###autoload
(defun mecab-new (&rest dictionaries)
  (if (or (null dictionaries) (stringp dictionaries))
      (mecab-core-new dictionaries)
    (mecab-core-new (apply #'vector dictionaries))))

(cl-defun mecab-sparse-to-string (mecab input &optional (limit -1))
  (cl-assert (not (null mecab)))
  (cl-assert (stringp input))
  (cl-assert (integerp limit))
  (mecab-core-sparse-to-string mecab input limit))

(cl-defun mecab-sparse-to-list (mecab input &optional (limit -1))
  (cl-assert (not (null mecab)))
  (cl-assert (stringp input))
  (cl-assert (integerp limit))
  (mecab-core-sparse-to-list mecab input limit))

(provide 'mecab)

;;; mecab.el ends here
