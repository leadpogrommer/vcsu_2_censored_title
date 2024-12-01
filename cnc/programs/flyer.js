var flyer = new LVGLLabel();
var stats = new LVGLLabel();

flyer.text = "X"
var x = 0;
var y = 0;
var dirx = 1;
var diry = 1


function f(){
    x += dirx;
    y += diry;
    if(x > 118 || x == 0) dirx = - dirx;
    if(y > 54 || y == 0) diry = - diry;
    flyer.x = x;
    flyer.y = y;
    stats.text = "x="+x+"\ny="+y;
    setTimeout(f, 100);
}

f()
