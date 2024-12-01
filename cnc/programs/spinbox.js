var lbl = new LVGLLabel();

var b = new LVGLButton(function f(){
    print('button pressed');
});
b.x = 70;
b.y = 15;
b.text = 'Btn';

var sb = new LVGLSpinbox(function (v){
    lbl.text = 'Value is ' + v;
}, 10, 90, 7, 2);

sb.y = 30;
sb.x = 15;
sb.w = 40;
