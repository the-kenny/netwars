;; make-qrc.lisp
;; Generates the data.qrc file from the data/ directory.
;; Run with sbcl --load make-qrc.lisp

(require 'cl-who)
(require 'cl-fad)

(use-package '(:cl-who :cl-fad))

#|
<!DOCTYPE RCC>
<RCC version="1.0">
  <qresource>
    <file>images/copy.png</file>
    <file>images/cut.png</file>
    <file>images/new.png</file>
    <file>images/open.png</file>
    <file>images/paste.png</file>
    <file>images/save.png</file>
   </qresource>
</RCC>
|#

(defun make-relative (path &optional (to *default-pathname-defaults*))
  (make-pathname 
    :directory (append '(:relative) 
                       (remove-if 
                         #'(lambda (f) 
                             (member f (pathname-directory to) 
                                     :test #'string=))
                         (pathname-directory path)))
    :name (pathname-name path)
    :type (pathname-type path)))

(defun replace-all (string part replacement &key (test #'char=))
  "Returns a new string in which all the occurences of the part 
is replaced with replacement."
  (with-output-to-string (out)
	(loop with part-length = (length part)
            for old-pos = 0 then (+ pos part-length)
	   for pos = (search part string
						 :start2 old-pos
						 :test test)
	   do (write-string string out
						:start old-pos
						:end (or pos (length string)))
	   when pos do (write-string replacement out)
	   while pos)))


(setf *prologue* "<!DOCTYPE RCC>")

(with-open-file (file "data.qrc" :direction :output :if-exists :supersede)
  (write-string (replace-all
				 (cl-who:with-html-output-to-string
					 (*standard-output* nil :prologue t)
				   (:RCC :version "1.0"
						 (:qresource
						  (walk-directory
						   #P"data/"
						   #'(lambda (file)
							   (htm (:file (str
											(namestring
											 (make-relative file))))))))))
				 "rcc"
				 "RCC")
				file))
