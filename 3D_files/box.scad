
$fn=200;
module bottom(){
linear_extrude(height = 2, center = false, convexity = 20)
		import(file = "dxf_plans.dxf", layer = "fond");
linear_extrude(height = 6, center = false, convexity = 20)
		import(file = "dxf_plans.dxf", layer = "cotes");
}

module top(){        
mirror([1,0,0]) union(){     
translate([5,0,0]) linear_extrude(height = 1, center = false, convexity = 20)
		import(file = "dxf_plans.dxf", layer = "top");
translate([5,0,0]) linear_extrude(height = 3, center = false, convexity = 20)
		import(file = "dxf_plans.dxf", layer = "topcote");
}
}

module lego(){
translate([5,0,0]) linear_extrude(height = 1, center = false, convexity = 20)
		import(file = "dxf_plans.dxf", layer = "Fixation");
translate([5,0,0]) linear_extrude(height = 3, center = false, convexity = 20)
		import(file = "dxf_plans.dxf", layer = "Fixationtetons");
}

bottom();
//top();