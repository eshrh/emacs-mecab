(require 'mecab)
(require 'cl-lib)

(let* ((mecab (mecab-new "/var/lib/mecab/dic/ipadic"))
       (lines (split-string (mecab-sparse-to-string mecab "俺がいる") "\n" t)))
  (cl-loop for line in lines
           for tokens = (split-string line "\t" t)
           unless (string= line "EOS")
           do
           (message "@@ %s:%s" (car tokens) (cadr tokens))))
