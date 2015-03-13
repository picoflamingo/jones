import cjones

def rule01 (r, o, f):
	print "Rule 00 fired!!!!"

def populate_kb ():
	print "Populating Knowledge Base"
	cjones.new_obj   ("John")
	cjones.new_obj   ("Sally")
	cjones.new_obj   ("Jane")

	#cjones.new_fact  ("John", "IS_MALE", "T")

	cjones.new_fact  ("Sally", "IS_FEMALE", "T")
	cjones.new_fact2 ("Sally", "IS_SISTER_OF", "Jane", "T")
	
	cjones.new_fact  ("Jane", "IS_FEMALE", "T")

	cjones.new_rule  ("rule01", "IS_BROTHER_OF")
#	cjones.new_obj   ("Penny")
#	cjones.new_fact  ("Penny", "IS_FEMALE", "T")
#	cjones.new_fact2 ("Penny", "IS_MOTHER_OF", "John", "T")

