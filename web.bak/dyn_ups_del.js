function del(ups, type) {
	//var tab1 = $(ups);
	var tab1 = $('table[name="' + ups +'"]');

	//document.getElementsByName(ups);
	var cols = tab1.children("tbody").children("tr").children("td").children();
	var arr = new Array();
	$(cols).each(function(){
		var jstr = getTableContent(this);
		jobj = JSON.parse(jstr);
		arr.push(jobj);
	});
}
