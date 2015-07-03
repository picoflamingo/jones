import cjones

# Battery Status rule
# Complementary facts CHARGED -> ! EMPTY ==> EMPTY -> ! CHARGED
# -----------------------------------------------------------------
def rule00 (r, o, f):
    if cjones.obj_get_fact_val (o, "IS_CHARGED") == 1:
        f2 = cjones.obj_get_or_create_fact (o, "IS_EMPTY", 0)
        cjones.fact_set (f2, 0)
        return 1
    else:
        f2 = cjones.obj_get_or_create_fact (o, "IS_EMPTY", 1)
        cjones.fact_set (f2, 1)
        return 1
    return 0

def rule01 (r, o, f):
    if cjones.obj_get_fact_val (o, "IS_EMPTY") == 1:
        f2 = cjones.obj_get_or_create_fact (o, "IS_CHARGED", 0)
        cjones.fact_set (f2, 0)
        return 1
    else:
        f2 = cjones.obj_get_or_create_fact (o, "IS_CHARGED", 1)
        cjones.fact_set (f2, 1)
        return 1
    return 0

# -----------------------------------------------------------------

# Automatic battery swap rule
#   Generic rule. Instead of working on the object received as parameter
#   Gets references to the relevant objects
#   Rule is associated to a fact on an object to avoid firing the rule
# on each single object
# --------------------------------------------
def rule02 (r, o, f):
    if cjones.fact_get (f):
        # Get references to relevant objects 
        o1 = cjones.find_obj ("MainBattery")
        o2 = cjones.find_obj ("AuxBattery")
        o3 = cjones.find_obj ("Payload")

        if cjones.obj_get_fact_val (o1, "IS_EMPTY") == 1:
            if cjones.obj_get_fact_val (o2, "IS_EMPTY") == 1:
                print "INFO:: No power available. Payload not available"
                f1 = cjones.obj_get_or_create_fact (o3, "IS_AVAILABLE", 0)
                cjones.fact_set (f1, 0)
                return 1
            else:
                print "INFO:: Switching to Aux Battery"
                f1 = cjones.obj_get_or_create_fact (o3, "USES_BATTERY", 1)
                cjones.fact_set (f1, 1)
                cjones.fact_set_robj (f1, o2, 0)
                f1 = cjones.obj_get_or_create_fact (o3, "IS_AVAILABLE", 1)
                cjones.fact_set (f1, 1)
                return 1
        else:  # If main battery is available switch to main battery
            print "INFO:: Switching to main Battery"
            f1 = cjones.obj_get_or_create_fact (o3, "USES_BATTERY", 1)
            cjones.fact_set (f1, 1)
            cjones.fact_set_robj (f1, o1, 0)
            f1 = cjones.obj_get_or_create_fact (o3, "IS_AVAILABLE", 1)
            cjones.fact_set (f1, 1)
            return 1
        return 0
    return 0

def populate_kb ():
	print "Populating Knowledge Base"
        
        # Create objects
	cjones.new_obj   ("Payload")
	cjones.new_obj   ("MainBattery")
	cjones.new_obj   ("AuxBattery")

        # Initial Facts 
        cjones.new_fact2 ("Payload", "USES_BATTERY", "MainBattery", "T")
        cjones.new_fact  ("MainBattery", "IS_EMPTY", "T")
        cjones.new_fact  ("AuxBattery", "IS_CHARGED", "T")

        # Rules
        # -----------------------------------------
        #  Battery status updates
        cjones.new_rule ("rule00", "IS_CHARGED")
        cjones.new_rule ("rule01", "IS_EMPTY")
        
        # Battery swap
        cjones.new_rule ("rule02", "USES_BATTERY")
