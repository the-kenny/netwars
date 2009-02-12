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


(setf *prologue* "<!DOCTYPE RCC>")
(with-open-file (file "data.qrc" :direction :output :if-exists :supersede)
  (cl-who:with-html-output (*standard-output* file :prologue t :indent nil)
                           (:RCC :version "1.0"
                                 (:qresource
                                   (walk-directory #P"data/" #'(lambda (file)
                                                                 (htm (:file (str (namestring (make-relative file)))))))))))

(print "You have to change rcc to RCC in data.qrc before it works!)
