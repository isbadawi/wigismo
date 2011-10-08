hangman.pl: hangman.stub generate.py style.css
	python2.7 generate.py 7 500
	lkwig hangman.wig
	cp hangman.pl $(HOME)/public_html/cgi-bin
	cp style.css $(HOME)/public_html
	cp -r img $(HOME)/public_html
	chmod -R 755 $(HOME)/public_html/
