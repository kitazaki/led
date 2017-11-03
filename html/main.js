var milkcocoa = new MilkCocoa('***.mlkcca.com');  // replace *** as MilkCocoa Application ID
var chatDataStore = milkcocoa.dataStore("");  // MilkCocoa Datastore Name
var board;
window.onload = function(){
  board = document.getElementById("board");
}
 
function clickEvent_white(){
  var v_r = 255; var v_g = 255; var v_b = 255;
  sendText(v_r, v_g, v_b);
}

function clickEvent_black(){
  var v_r = 0; var v_g = 0; var v_b = 0;
  sendText(v_r, v_g, v_b);
}

function clickEvent_blue(){
  var v_r = 0; var v_g = 0; var v_b = 255;
  sendText(v_r, v_g, v_b);
}

function clickEvent_yellow(){
  var v_r = 255; var v_g = 255; var v_b = 0;
  sendText(v_r, v_g, v_b);
}

function clickEvent_red(){
  var v_r = 255; var v_g = 0; var v_b = 0;
  sendText(v_r, v_g, v_b);
}
 
function sendText(v_r, v_g, v_b){
  chatDataStore.push({r : v_r, g : v_g, b : v_b},function(data){
    console.log("送信完了!");
  });
}
 
chatDataStore.on("push",function(data){
  addText(data.value.r, data.value.g, data.value.b);
});
 
function addText(v_r, v_g, v_b){
  var msgDom = document.createElement("li");
  msgDom.innerHTML = v_r + "," + v_g + "," + v_b;
  board.insertBefore(msgDom, board.firstChild);
}

