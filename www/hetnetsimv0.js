$(document).ready(function() {
    $('form').submit(function(event)
    {
        /* var problem = $('#objeqn').val() + ";" + $('#ineq1').val() + "," + 
                         $('#ineq2').val() + "," + $('#ineq3').val() + ",;;";
        $.post('index.html', {"message" : $('#problem').val()}, null, 'json');*/

        $.ajax({
            type     : 'POST',
            url      : 'process.php',
            data     : {'problem' : $('#problem').val()},
            dataType : 'json',
            success  : function(data) {
                if (data.success) {
                    $('#result').append("<p>" + data.posted + "</p>");
                } else {
                    console.log("failure");
                }
            }
        });
        event.preventDefault();
    });
});
