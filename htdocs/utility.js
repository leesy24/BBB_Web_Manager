function isInValidRange(s,low,high) {
 	if((isNaN(s))||(s<low||s>high))
    	return false;
	else
		return true;
}

function isBlank(s) {
	for(i=0;i<s.length;i++) {
		c = s.charAt(i);
		if( (c!=' ')&&(c!='\n')&&(c!='\t') )
			return false;
	}return true;
}

function cutleadingzeros(s){
	x = 0;
	while (st.charAt(x) == '0')
		x++;
	if (st.length != 1) 
	st = st.substring(x, st.length);
}

function isValidAdminPassword(s){
	if (s.length < 8) 
		return false;
	else{
		for(i=0;i<s.length;i++) {
		c = s.charAt(i);
		if (c == '&')
		return false;
	}
	}
	return true;
}

function isValidUserPassword(s){
	for(i=0;i<s.length;i++) {
		c = s.charAt(i);
		if (c == '&')
		return false;
	}
	return true;
}

function isStringsEqual(s1,s2){
	if (s1.length != s2.length)
		return false;
	for(i=0;i<s1.length;i++) {
		if (s1.charAt(i) != s2.charAt(i))
			return false;
	}
	return true;
}

function isValidIPorEmpty(s) {
 tempstr = s;
 if (isBlank(s))
    return true;
 else
 	{
		var IParray = s.split(".");

		if ((IParray.length>4)||(IParray.length<2)) 
			return false;

		for (i=0;i<IParray.length;i++)
		{
			st = IParray[i];
			if (isNaN(st)||(st=="")){
				return false;
			}
			if ((st<0)||(st>255)) 
				return false;

		cutleadingzeros(st);
		IParray[i] = st;
		}

	
		if (IParray.length==2)
		{
			IParray[3] = IParray[1];
			IParray[1] = 0;
			IParray[2] = 0;
		}
		else if (IParray.length==3)
		{
			IParray[3] = IParray[2];
			IParray[2] = 0;
		}
		if (((IParray[3]==0)||(IParray[3]==255))&&(tempstr!="0.0.0.0"))
			return false;

		tempstr = IParray.join(".");
	}
  return true;
}


function isValidIP(s) {
 tempstr = s;
 if (isBlank(s))
    return false;
 else
 	{
		var IParray = s.split(".");

		if ((IParray.length>4)||(IParray.length<2)) 
			return false;

		for (i=0;i<IParray.length;i++)
		{
			st = IParray[i];
			if (isNaN(st)||(st=="")){
				return false;
			}
			if ((st<0)||(st>255)) 
				return false;

		cutleadingzeros(st);
		IParray[i] = st;
				
		}

	
		if (IParray.length==2)
		{
			IParray[3] = IParray[1];
			IParray[1] = 0;
			IParray[2] = 0;

		}
		else if (IParray.length==3)
		{
			IParray[3] = IParray[2];
			IParray[2] = 0;

		}
		tempstr = IParray.join(".");

	}
  return true;
}

function isValidMask(s) {
 tempstr = s;
 if (isBlank(s))
    return false;
 else
 	{
		var IParray = s.split(".");

		if (IParray.length!=4)
			return false;

		for (i=0;i<4;i++)
		{
			st = IParray[i];
			if (isNaN(st)||(st=="")){
				return false;
			}
			if ((st<0)||(st>255)) 
				return false;
		cutleadingzeros(st);
		IParray[i] = st;
		}
		tempstr = IParray.join(".");	
	}
  return true;
}