import cjones

# Rule IS_SISTER_OF
def rule00 (r, o, f):
	if cjones.fact_get (f):
		# if o IS_SISTER_OF o1 ==> o IS_FEMALE
		f1 = cjones.obj_get_or_create_fact (o, "IS_FEMALE", 1)
		# if o IS_SISTER_OF o1  ==> o1 IS_SISTER_OF o <==> o IS_FEMALE
		#o1 = cjones.fact_get_obj (f, 0);
		indx = 0
		while True:
			o1 = cjones.fact_get_obj (f, indx);
			if o1 == 0:
				break;
			if cjones.obj_get_fact_val (o1, "IS_FEMALE") == 1 :
				f2 = cjones.obj_get_or_create_fact (o1, "IS_SISTER_OF", 1)
				cjones.fact_add_obj (f2, o)
			elif cjones.obj_get_fact_val (o1, "IS_MALE") == 1: 
				f2 = cjones.obj_get_or_create_fact (o1, "IS_BROTHER_OF", 1)
				cjones.fact_add_obj (f2, o)
			indx = indx + 1;
		return 1
	else:
		return 0



# Rule IS_BROTHER_OF
def rule01 (r, o, f):
	if cjones.fact_get (f):
		# if o IS_BROTHER_OF o1 ==> o IS_MALE
		f1 = cjones.obj_get_or_create_fact (o, "IS_MALE", 1)
		# if o IS_BROTHER_OF o1  ==> o1 IS_BROTHER_OF o <==> o IS_MALE
		#o1 = cjones.fact_get_obj (f, 0);
		#print "rexp object: %d" % o1
		indx = 0
		while True:
			o1 = cjones.fact_get_obj (f, indx);
			if o1 == 0:
				break;
			if cjones.obj_get_fact_val (o1, "IS_MALE") == 1 :
				f2 = cjones.obj_get_or_create_fact (o1, "IS_BROTHER_OF", 1)
				cjones.fact_add_obj (f2, o)
			elif cjones.obj_get_fact_val (o1, "IS_FEMALE") == 1: 
				f2 = cjones.obj_get_or_create_fact (o1, "IS_SISTER_OF", 1)
				cjones.fact_add_obj (f2, o)
			indx = indx + 1;
		return 1
	else:
		return 0

# Gender Rule
def rule02 (r, o, f):
	if cjones.obj_get_fact_val (o, "IS_MALE") == 1:
		f2 = cjones.obj_get_or_create_fact (o, "IS_FEMALE", 0)
		return 1
	if cjones.obj_get_fact_val (o, "IS_FEMALE") == 1:
		f2 = cjones.obj_get_or_create_fact (o, "IS_MALE", 0)
		return 1
	return 0



def rule03 (r, o, f):
	if cjones.fact_get (f):
		# if o IS_SYSTER_OF o1, o2, o3 ==> o1 IS_SISTER/BROTHER o2....

		indx = 0
		while True:
			o1 = cjones.fact_get_obj (f, indx);
			if o1 == 0:
				break;
			indx1 = 0
			while True:
				if indx == indx1:
					break
				o2 = cjones.fact_get_obj (f, indx1);
				if o2 == 0:
					break;

				if cjones.obj_get_fact_val (o1, "IS_MALE") == 1 :
					f2 = cjones.obj_get_or_create_fact (o1, "IS_BROTHER_OF", 1)
					cjones.fact_add_obj (f2, o2)
				elif cjones.obj_get_fact_val (o1, "IS_FEMALE") == 1: 
					f2 = cjones.obj_get_or_create_fact (o1, "IS_SISTER_OF", 1)
					cjones.fact_add_obj (f2, o2)
				indx1 = indx1 + 1;
			indx = indx + 1;
		return 1
	else:
		return 0





def populate_kb ():
	print "Populating Knowledge Base"
	cjones.new_obj   ("John")
	cjones.new_obj   ("Sally")
	cjones.new_obj   ("Jane")

	cjones.new_fact  ("John", "IS_MALE", "T")
	cjones.new_fact2  ("John", "IS_BROTHER_OF", "Sally", "T")

	cjones.new_fact  ("Sally", "IS_FEMALE", "T")
	cjones.new_fact2 ("Sally", "IS_SISTER_OF", "Jane", "T")
	
	cjones.new_fact   ("Jane", "IS_FEMALE", "T")
	#cjones.new_fact2  ("Jane", "IS_SISTER_OF", "John", "T")


	# Rules
	# -----------------------------------------------
	cjones.new_rule  ("rule00", "IS_SISTER_OF")
	cjones.new_rule  ("rule01", "IS_BROTHER_OF")

	# Complementary fact rule
	cjones.new_rule  ("rule02", "IS_MALE")
	cjones.new_rule  ("rule02", "IS_FEMALE")

	cjones.new_rule  ("rule03", "IS_SISTER_OF")
#	cjones.new_obj   ("Penny")
#	cjones.new_fact  ("Penny", "IS_FEMALE", "T")
#	cjones.new_fact2 ("Penny", "IS_MOTHER_OF", "John", "T")

