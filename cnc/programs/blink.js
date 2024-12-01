function lbl(txt, x, y){
    var l = new LVGLLabel();
    l.text = txt;
    l.x = x;
    l.y = y;
}

var pin = 2;
var on = 100;
var off = 100;

lbl('Pin:', 0, 0)
lbl('On, ms', 0, 15)
lbl('Off, ms', 0, 30)

var sb_pin = new LVGLSpinbox(
    function (p){pin = p},
    2, 4, 1, 1
)
sb_pin.y = 0;
sb_pin.x = 70;
sb_pin.w = 50;

var sb_on = new LVGLSpinbox(
    function (p){on = p},
    100, 999, 10, 3
)
sb_on.y = 15;
sb_on.x = 70;
sb_on.w = 50;

var sb_off = new LVGLSpinbox(
    function (p){off = p},
    100, 999, 10, 3
)
sb_off.y = 30;
sb_off.x = 70;
sb_off.w = 50;

var is_running = false;
var is_on = false;


function timeout_fun(){
    if(!is_running) return;
    is_on = !is_on;
    digitalWrite(pin, is_on);
    setTimeout(timeout_fun, is_on ? on : off);
}

// TODO: it's possible to start timeout twice
function update_state(){
    if (is_running){
        // start
        btn.text = 'Stop'
        pinMode(pin, 0);
        timeout_fun();
    } else {
        // stop
        btn.text = 'Start'
    }
}

var btn = new LVGLButton(function () {
    is_running = !is_running;
    update_state();
});
btn.x = 0;
btn.y = 45;

update_state();