var cntr = 0;

var lbl = new LVGLLabel();
lbl.x = 100;
lbl.y = 45;
lbl.text = '0';

function upd(){
    lbl.text = '' + cntr;
}

function inc(){
    cntr++;
    upd();
}
function dec(){
    cntr--;
    upd();
}
function reset(){
    cntr = 0;
    upd();
}

var btn = new LVGLButton(inc);
btn.x = 10;
btn.y = 10;
btn.text = '+'

var btn = new LVGLButton(dec);
btn.x = 10;
btn.y = 25;
btn.text = '-'

var btn = new LVGLButton(reset);
btn.x = 10;
btn.y = 40;
btn.text = '0'

