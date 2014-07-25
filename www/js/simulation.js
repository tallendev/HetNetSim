/*
 * File: simulation.js
 * Last Edit: 7/25/2014
 * Author: Matthew Leeds
 * Author: Tyler Allen
 * Purpose: This script is the backend for the HetNet Simulation in index.html.
 * It provides interactivity and communicates with the server to solve 
 * linear programming problems. The main() function sets up the canvas and default
 * values for variables. The changeMode() function is the controller that 
 * calls appropriate functions to implement the modes. Pretty much every other
 * function is a helper for those two.
 */

// document level variables
var backgroundColor;
var canvas;
var borderPath;
var w;
var h;
var x;
var y;
var xAxis;
var yAxis;

// simulation level variables
var buttonFuncs; // used to translate shapes to functions that make them
var colorArray; // holds the names of each available color
var color; // current color
var shape; // the currently selected shape (circle or triangle)
var mode; // the current mode
var deviceID; // used to assign IDs to devices (device1, device2, etc.)
var networkID; // used to assign IDs to networks (network1, network2, etc.)
var maxRates; // holds the max achievable data rate for each network
var instructions; // dictionary with instructions for each mode
var textBox; // used to display instructions
var alpha; // weight of aggregate throughput in the optimization
var beta;  // weight of fairness in the optimization
var sumRuaMax; // sum of the max achievable rates for every device-network pair
var simData; // holds r_ua,max values for each device-network pair
var numDevices; // these are useful when formulating the optimization problem,
var numNetworks; // and making sense of the answer that's returned.

// constants
var TRANSITION_DURATION = 500; // # of ms elements take to fade in or out
var OPACITY_LEVEL = 0.5;       // full opacity level
var DEVICES_COLOR = "darkgray";  
var BORDER_WIDTH = 5;         
var BORDER_COLOR = "gray";    
var GRIDLINE_GRANULARITY = 20; // determines fine/coarse gridlines


/*
 * The main function sets up the canvas, its border and axes,
 * and the default choices in the side menu.
 */
function main()
{
    w = $("body").width() - 260; // account for the width of the sidebar
    h = $("body").height();

    canvas = d3.select("#simulation")
               .append("svg")
               .attr("width", w)
               .attr("height", h);    

    canvas.append("g").attr("id", "layer1"); // to control ordering of elements
    canvas.append("g").attr("id", "layer2");
    canvas.append("g").attr("id", "layer3");
    canvas.append("g").attr("id", "layer4");

    textBox = canvas.select("#layer4")
                    .append("text")
                    .attr("x", w / 2)
                    .attr("y", 40)
                    .attr("fill", "darkgray")
                    .attr("font-size", "medium")
                    .attr("font-weight", "bold")
                    .attr("text-anchor", "middle")
                    .text("");

    buttonFuncs = {'circle' : genCircle, 
                   'triangle' : genTriangle}
    colorArray = ["red", "green", "blue"];
    color = colorArray[0];
    maxRates = {};
    instructions = {"Add"     : "Click anywhere to add a network or device.",
                    "Resize"  : "Click and drag on any network to resize it.",
                    "Move"    : "Click and drag on any network to move it.",
                    "Delete"  : "Click on any network or device to delete it.",
                    "Filter"  : "Filter networks and devices using the switches.",
                    "Optimize": "Adjust the slider to change the weights of optimization parameters."
                   }
    shape = "circle";
    $('input[value="Add"]').prop('checked', true);
    $('input[value="circle"]').prop('checked', true);
    $('#color1').prop('checked', true);
    changeMode();

    deviceID = 0;
    networkID = 0;

    backgroundColor = $("body").css("background-color");

    x = d3.scale.linear().range([0, w]);
    y = d3.scale.linear().range([h, 0]); 
    xAxis = d3.svg.axis().scale(x).orient("bottom")
                                  .ticks(w / GRIDLINE_GRANULARITY)
                                  .tickSize(-h,0,0)
                                  .tickFormat("");
    yAxis = d3.svg.axis().scale(y).orient("left")
                                  .ticks(h / GRIDLINE_GRANULARITY)
                                  .tickSize(-w,0,0)
                                  .tickFormat("");
    drawAxes();

    borderPath = canvas.select("#layer1")
                       .append("rect")
                       .attr("x", 0)
                       .attr("y", 0)
                       .attr("width", w)
                       .attr("height", h)
                       .style("stroke", BORDER_COLOR)
                       .style("fill", "none")
                       .style("stroke-width", BORDER_WIDTH);

}

/*
 * distance uses the pythagorean theorem to calculate the magnitude of the 
 * distance between two points.
 */
function distance(x1, y1, x2, y2)
{
    return Math.sqrt(
           Math.pow(Math.abs(x2 - x1), 2) +
           Math.pow(Math.abs(y2 - y1), 2));
}

/*
 * The drawAxes function appends the x and y axes to the canvas
 * and translates the x axis so it starts in the bottom left instead 
 * of the top left.
 */
function drawAxes() {
    canvas.select("#layer1").append("g")
                            .attr("class", "grid")
                            .attr("transform", "translate(0," + h + ")")
                            .call(xAxis);
    canvas.select("#layer1").append("g")
                            .attr("class", "grid")
                            .call(yAxis);
}


/*
 * gatherData analyzes the simulation to collect the normalized distances
 * between every device and access point, and calculates maximum achievable 
 * data rates using r_ua,max = r_a,max / (1 - d_ua,norm)^2. These are stored in 
 * the simData json object. See the documentation for more information. 
 */ 
function gatherData()
{
    simData = {}; // will contain max data rates for every network-device pair
    sumRuaMax = 0;
    for (var i = 1; i <= deviceID; i++)
    {
        var currentDeviceID = "#device" + i;
        if (!d3.select(currentDeviceID).empty()) // make sure it wasn't deleted
        {
            var deviceInfo = {}; // will contain max data rates for each network
            var xforms = $(currentDeviceID).parent().prop("transform").baseVal;
            for (var j = 1; j <= networkID; j++)
            {
                var currentNetworkID = "#network" + j;
                if (!d3.select(currentNetworkID).empty()) // make sure it wasn't deleted
                {
                    var distanceFromPoint = distance(xforms.getItem(0).matrix.e, 
                                                     xforms.getItem(0).matrix.f,
                                                     $(currentNetworkID).prop("cx").baseVal.value,
                                                     $(currentNetworkID).prop("cy").baseVal.value);
                    var radius = $(currentNetworkID).prop("r").baseVal.value;
                    if (distanceFromPoint < radius)
                    {
                        var ruaMax = maxRates[currentNetworkID] * 
                                     Math.pow((1 - (distanceFromPoint / radius)), 2);
                        sumRuaMax += ruaMax;
                        deviceInfo[currentNetworkID] = ruaMax;
                    }
                    else
                    {
                        deviceInfo[currentNetworkID] = 0;
                    }
                }
            }
            simData[currentDeviceID] = deviceInfo;
        }
    }
    console.log(simData);
}


/*
 * The genCircle function can be called to generate new circle elements.
 * It takes in the svg, location, radius, and color.
 */    
function genCircle(svg, cx, cy, radius, newColor)
{
    var theColor = d3.rgb(newColor);
    networkID++;
    inputStr = prompt("Please enter the maximum possible bandwidth rate for " 
                    + "this network (default 1000): ");
    if (parseFloat(inputStr) > 0)
        maxRates["#network" + networkID] = parseFloat(inputStr);
    else
        maxRates["#network" + networkID] = 1000;

    svg.select("#layer2").append("circle")
                         .attr("id", "network" + networkID)
                         .attr("cx", cx)
                         .attr("cy", cy)
                         .attr("r", radius)
                         .attr("stroke", theColor.darker(.9))
                         .attr("stroke-width", radius / 10)
                         .attr("opacity", OPACITY_LEVEL)
                         .style("fill", theColor);
}


/* 
 * The genTriangle function generates triangles using paths.
 * It ignores the color parameter which is there for consistency with
 * genCircle so either can be called with one statement. The triangle has to
 * be translated to the appropriate place because symbols start at the origin.
 */
function genTriangle(svg, xCoord, yCoord, size, newColor)
{
    deviceID++;

    // make a container to hold the triangle and text box
    var thisLayer = svg.select("#layer3")
                       .append("g") 
                       .attr("id", "device" + deviceID + "container")
                       .attr("transform", function() {
                           return "translate(" + xCoord + "," + yCoord + ")";
                       });
    
    // add the triangle element 
    thisLayer.append("path")
             .attr("id", "device" + deviceID)
             .attr("class", "point")
             .style("fill", DEVICES_COLOR)
             .attr("d", d3.svg.symbol().type("triangle-up"))
             .attr("size", size);
    
    // the text box will be added when the optimization happens
}

/*
 * changeText transitions the opacity of the text box and changes its text
 * based on the current mode.
 */
function changeText(textContent)
{
    textBox.style("opacity", 0);
    textBox.text(textContent)
           .transition()
           .style("opacity", 1)
           .duration(TRANSITION_DURATION);
    textBox.transition()
           .style("opacity", 0)
           .duration(TRANSITION_DURATION)
           .delay(3000) // show text for 3 seconds
           .text("");
}

/*
 * modeAdd enables the add functionality on the canvas and
 * shows the shape and color options to the user.
 */
function modeAdd()
{
    $('#shape').show();
    changeShape();
    canvas.on("click", function()
    {
        drawFunc = buttonFuncs[shape];
        if (drawFunc != null)
        {
            drawFunc(canvas, d3.mouse(this)[0], 
                             d3.mouse(this)[1], 50, color);
        }
    });
}


/*
 * modeResize enables resizing functionality by calling the
 * resize function when a circle is dragged.
 */
function modeResize()
{
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", resize));
}


/*
 * modeMove allows users to move shapes by changing the drag behavior. 
 * For circles, the origin is set as the location of the circle at
 * the start of the drag so x and y coordinates are relative to that, which
 * makes the motion smoother.
 */
function modeMove()
{
    d3.selectAll("circle").call(d3.behavior.drag()
                                           .origin(circleOrigin)
                                           .on("drag", dragCircle));
    d3.selectAll("path").call(d3.behavior.drag()
                                          .on("drag", dragPath));
}


/*
 * modeDelete lets users delete elements making use of d3's remove function.
 */
function modeDelete()
{
    d3.selectAll("circle").on("click", function()
                              {
                                  d3.select(this).remove();
                              });

    d3.selectAll(".point").on("click", function()
                              {
                                  d3.select(this.parentNode).remove();
                              });
}


/*
 * modeFilter shows the switchs that let the user change the parameters
 * of the filter and calls updateFilter, which makes the canvas reflect those
 * selections.
 */
function modeFilter()
{
    $('#filters').show();
    updateFilter();
}


/*
 * modeOptimize shows the optimization options and calls the functions
 * that collect data about the distances between each device-network pair
 * and make sure the user-defined weights are recorded internally.
 */
function modeOptimize()
{
    $('#optimization').show();
    gatherData();
    updateParams();
}

/*
 * changeMode is called whenever the user clicks on a mode, and once when
 * the page loads. It removes all behaviors on the canvas and enables 
 * appropriate ones using the functions above.
 */
function changeMode()
{
    mode = $('input[name=mode]:checked').val();
    
    $('#shape').hide();
    $('#color').hide();
    $('#filters').hide();
    $('#optimization').hide();
    showAll(); // shows all elements (circles and triangles)
    removeBehaviors(); // disables click and drag behaviors
    removeTextBoxes(); // removes any text boxes from optimizations

    switch (mode)
    {
        case "Add":
        {
            modeAdd();
            break;
        }
        case "Resize":
        {
            modeResize();
            break;
        }
        case "Move":
        {
            modeMove();
            break;
        }
        case "Delete":
        {
            modeDelete();
            break;
        }
        case "Filter":
        {
            modeFilter();
            break;
        }
        case "Optimize":
        {
            modeOptimize();
            break;
        }
    }

    changeText(instructions[mode]);
}


/*
 * changeShape is called when the user selects a shape. It updates the 
 * global shape variable and decides whether to show the color options.
 */
function changeShape()
{
    shape = $('input[name=shape]:checked').val();
    if (shape === "circle") 
        $('#color').show();
    else
        $('#color').hide();
}


/*
 * changeColor is called when the user selects a color, and updates the 
 * global color variable.
 */
function changeColor()
{
    color = $('input[name=color]:checked').val();
}


/*
 * resize uses the pythagorean theorem to calculate the cursor's distance 
 * from the center of the circle, and changes the radius appropriately.
 */
function resize()
{
    var thisCircle = d3.select(this);
    var mouse = d3.mouse(this);
    var newRadius = distance(thisCircle.attr("cx"),
                             thisCircle.attr("cy"),
                             mouse[0],
                             mouse[1]);
    thisCircle.attr("r", newRadius);
}


/*
 * Drag behaviors can only give x and y coordinates relative to an origin if 
 * that origin object has x and y attributes, so this function is necessary 
 * to bridge that gap.
 */
function circleOrigin()
{
    return { x: d3.select(this).attr("cx"),
             y: d3.select(this).attr("cy") };
}


/* 
 * dragCircle changes the selected circle's position based on the drag event's
 * coordinates, which are offsets based on its original position.
 */
function dragCircle()
{
    d3.select(this).attr("cx", d3.event.x)
                   .attr("cy", d3.event.y);
}


/*
 * dragPath is used to move paths (triangles in this case), using the 
 * transform attribute. The parent element, an svg:g container, is the 
 * one that needs to move so the text box moves with it.
 */
function dragPath()
{
    var thisID = d3.select(this).attr("id");
    var startX = $(this).parent().prop("transform").baseVal.getItem(0).matrix.e;
    var startY = $(this).parent().prop("transform").baseVal.getItem(0).matrix.f;
    d3.select("#" + thisID + "container").attr("transform", function()
    {
        var moveX = d3.event.x + startX;
        var moveY = d3.event.y + startY;
        return "translate(" + moveX + "," + moveY + ")";
    });
}


/*
 * updateFilter makes all elements visible, and if the mode is set to filter,
 * it selectively hides appropriate elements. To make it smooth, the opacity
 * is transitioned to 0 over a short duration, and then the visibility is
 * set to hidden, which ensures that click or drag events cannot occur.
 * To hide the correct colors and stay "scalable", it iterates over the
 * colorArray, and assumes a switch has been defined for each in the HTML
 * after the network and devices switches.
 */
function updateFilter()
{
    showAll();
    if (mode === "Filter")
    {
        if (!$('#switch1').prop('checked'))
        {
            d3.selectAll("circle").transition()
                                  .style("opacity", 0)
                                  .duration(TRANSITION_DURATION)
                                  .transition()
                                  .style("visibility", "hidden")
                                  .duration(0);
        }
        if (!$('#switch2').prop('checked'))
        {
            d3.selectAll("path").transition()
                                .style("opacity", 0)
                                .duration(TRANSITION_DURATION)
                                .transition()
                                .style("visibility", "hidden")
                                .duration(0);
        }
        for (var i = 0; i < colorArray.length; i++)
        {
            var switchID = '#switch' + (i+3).toString().replace(/^[0]+/g,"");
            if (!$(switchID).prop('checked'))
            {
                console.log("on " + switchID);
                d3.selectAll("circle").filter( function()
                {
                    var currentColor = d3.rgb(d3.select(this).style("fill"));
                    var filterColor = d3.rgb($(switchID).attr("value")
                                                    .toLowerCase());
                    if (currentColor.toString() == filterColor.toString())
                        return d3.select(this);
                }).transition()
                  .style("opacity", 0)
                  .duration(TRANSITION_DURATION);
            }
        }                                
    } 
}


/*
 * showAll smoothly transitions all elements to full opacity/color 
 * to make them visible.
 */
function showAll()
{
    d3.selectAll("circle").transition()
                          .style("opacity", OPACITY_LEVEL)
                          .duration(TRANSITION_DURATION)
                          .style("visibility", "visible");
    d3.selectAll("path").transition()
                        .style("opacity", 1)
                        .duration(TRANSITION_DURATION)
                        .transition() // so they disappear immediately the first time
                        .style("visibility", "visible")
                        .duration(0);
}


/*
 * removeBehaviors removes all click and drag behaviors from the canvas
 * and elements to ensure that none persist into the wrong mode.
 */
function removeBehaviors()
{
    canvas.on("click", null);
    d3.selectAll("circle").call(d3.behavior.drag().on("drag", null));
    d3.selectAll("path").call(d3.behavior.drag().on("drag", null));
    d3.selectAll("circle").on("click", null);
    d3.selectAll("path").on("click", null)
                        .on("mouseover", null)
                        .on("mouseout", null);
}


/*
 * removeTextBoxes is called whenever the mode changes to make sure the 
 * text boxes that hold network allocations from optimization don't persist
 * when the user changes back to another mode.
 */
function removeTextBoxes()
{
    for (var i = 1; i <= deviceID; i++)
    {
        var currentDeviceID = "#device" + i;
        if (!d3.select(currentDeviceID).empty())
        {
            if ($(currentDeviceID).siblings().length > 0) 
            {
                d3.select(currentDeviceID).node().nextSibling.remove(); // rect
                d3.select(currentDeviceID).node().nextSibling.remove(); // text
            }
        }
    }
}


/*
 * updateParams updates the variable used to weight fairness and aggregate
 * throughput, alpha and beta, and reflects those values in the sidebar.
 */
function updateParams()
{
   beta = parseFloat($('#param1').val()).toFixed(2);
   alpha = (1 - Number(beta)).toFixed(2);
   $('#fairnessVal').html("Fairness Weight: " + beta);
   $('#throughputVal').html("Throughput Weight: " + alpha);
}


/*
 * optimize uses the values of alpha and beta, and the data in simData to 
 * form a linear programming problem to optimize the system. Then it calls 
 * the LPSolver written in C++ using a shared library available to PHP on
 * the server side. The format for the problem passed to the solver is: 
 * "objeqn;ineq,ineq,;eq,eq,;" where equations are numbers (coefficients) 
 * separated by spaces.
 */
function optimize()
{
    var problemFormulation = "";
    numDevices = Object.keys(simData).length; // not always equal to deviceID
    if (numDevices == 0)
    {
        alert("Please add networks and devices before attempting to optimize "
            + "the system.");
        return;
    }

    // formulate the objective equation, alpha * sum(r_u) + beta * sum(r_ua,max) * z
    var objEqn = "";
    for (var device in simData)
    {
        if (simData.hasOwnProperty(device))
        {
            for (var network in simData[device])
            {
                if (simData[device].hasOwnProperty(network))
                {
                    objEqn += String(alpha * simData[device][network]);
                    objEqn += " ";
                }
            }
        }
    }
    if (objEqn == "") // there are no networks
    {
        alert("Please add networks and devices before attempting to optimize "
            + "the system.");
        return;
    }
    objEqn += String(beta * sumRuaMax);
    problemFormulation += objEqn + ";";

    // formulate constraints to ensure networks don't exceed max bandwidth
    var inequalities = "";
    numNetworks = Object.keys(simData[Object.keys(simData)[0]]).length;
    for (var i = 0; i < numNetworks; i++)
    {
        for (var j = 0; j < numDevices; j++)
        {
            var currentDevice = simData[Object.keys(simData)[j]];
            for (var k = 0; k < numNetworks; k++)
            {
                if (k == i)
                    inequalities += String(currentDevice[Object.keys(currentDevice)[k]]) + " ";
                else
                    inequalities += "0 ";
            }
        }
        inequalities += "0 "; // z's coefficient
        var currentNetworkID = "#network" + String(i + 1);
        inequalities += String(maxRates[currentNetworkID]) + ",";
    }
   
    // formulate constraints to ensure each x_ua <= 1 so devices don't exceed
    // their potential bandwidth
    var equalities = "";
    for (var i = 0; i < numDevices * numNetworks; i++)
    {
        var assignmentConstraint = "";
        for (var j = 0; j < numDevices; j++)
        {
            for (var k = 0; k < numNetworks; k++)
            {
                if ((numNetworks * j) + k == i)
                    assignmentConstraint += "1 ";
                else
                    assignmentConstraint += "0 ";
            }
        }
        var currentDeviceIndex = Math.floor(i / numNetworks);
        var currentDevice = simData[Object.keys(simData)[currentDeviceIndex]];
        var currentMax = currentDevice[Object.keys(currentDevice)[i % currentDeviceIndex]];
        if (currentMax == 0)
        {
            assignmentConstraint += "0 0,";
            equalities += assignmentConstraint; 
        }
        else
        {
            assignmentConstraint += "0 1,";
            inequalities += assignmentConstraint;
        }
    }

    // formulate constraints to ensure z <= r_u for all devices
    for (var i = 0; i < numDevices; i++)
    {
        var currentDevice = simData[Object.keys(simData)[i]];
        for (var j = 0; j < numDevices; j++)
        {
            for (var k = 0; k < numNetworks; k++)
            {
                if (j == i)
                {
                    inequalities += String(-1 * currentDevice[Object.keys(currentDevice)[k]]);
                    inequalities += " ";
                }
                else
                {
                    inequalities += "0 ";
                }
            }
        }
        inequalities += "1 0,"; // 1 is z's coefficient, eqn <= 0
    }
    problemFormulation += inequalities + ";" + equalities + ";";
    console.log(problemFormulation);

    // make an AJAX POST to the server with the problem information
    $.ajax({
        type     : 'POST',
        url      : 'process.php',
        data     : {'problem' : problemFormulation},
        dataType : 'json',
        success  : function(data) {
            if (data.success) {
                console.log(data.answer);
                visualSolution(data.answer);
                changeText("Hover over devices to see their assigned "
                         + "allocations for each network.");
            } else {
                console.log("POST in function 'optimize()' failed.");
            }
        }
    });
}


/*
 * visualSolution takes the string returned by the Linear Programming solver
 * that has the optimal values of all the decision variables, calculates 
 * effective data transfer rates by multiplying each assignment value by
 * that devices maximum achievable rate, and puts these rates into the 
 * text boxes associated with each device.
 */
function visualSolution(solutionString)
{
    // account for the answer's quirky formatting
    var start = solutionString.indexOf('>') + 17;
    solutionString = solutionString.substr(start, solutionString.length - start - 2);
    var answerValues = solutionString.split(' ');
    console.log(answerValues);
    var lineHeight = 20;
    var textBoxHeight = (numNetworks + 1) * lineHeight;
    var textBoxWidth = 180;
    var foundDevices = 0; // so indexes aren't wrong when devices are deleted
    for (var i = 0; i < deviceID; i++)
    {
        var currentDeviceID = "#device" + String(i + 1);
        if (!d3.select(currentDeviceID).empty()) // make sure it wasn't deleted
        {
            foundDevices++;
            var thisContainer = d3.select(currentDeviceID + "container");

            // add a box as a background for the text
            thisContainer.select("rect").remove();
            thisContainer.append("rect")
                         .attr("id", currentDeviceID.substr(1) + "rect")
                         .attr("width", textBoxWidth)
                         .attr("height", textBoxHeight)
                         .attr("x", 5)
                         .attr("y", -textBoxHeight)
                         .attr("rx", 10)
                         .attr("ry", 10)
                         .style("fill", "lightsteelblue")
                         .style("opacity", 0);
            
            // add a text element to display network rate assignments
            thisContainer.select("text").remove();
            thisContainer.append("text")
                         .attr("id", currentDeviceID.substr(1) + "text")
                         .attr("fill", "black")
                         .attr("text-anchor", "left")
                         .attr("font-size", "15px")
                         .style("opacity", 0);
            
            // text boxes should only appear when you hover over that device       
            d3.select(currentDeviceID).on("mouseover", function() {
                                            d3.select(this.nextSibling)
                                            .transition()
                                            .style("opacity", 1)
                                            .duration(200);
                                            d3.select(this.nextSibling.nextSibling)
                                            .transition()
                                            .style("opacity", 1)
                                            .duration(200);
                                          })
                                      .on("mouseout", function() {
                                            d3.select(this.nextSibling)
                                            .transition()
                                            .style("opacity", 0)
                                            .duration(200);
                                            d3.select(this.nextSibling.nextSibling)
                                            .transition()
                                            .style("opacity", 0)
                                            .duration(200);
                                          });
            
            var deviceInfo = simData[currentDeviceID]; // get r_ua,max values
            var foundNetworks = 0; // so text is spaced regardless of deleted networks
            for (var j = 0; j < networkID; j++)
            {
                var currentNetworkID = "#network" + String(j + 1);
                if (!d3.select(currentNetworkID).empty()) // it wasn't deleted
                {
                    foundNetworks++;
                    var answerIndex = ((foundDevices - 1) * numNetworks) + (foundNetworks - 1);
                    var assignedRate = answerValues[answerIndex] * deviceInfo[currentNetworkID];
                    var yOffset = (lineHeight * foundNetworks) - textBoxHeight;
                    d3.select(currentDeviceID + "text").append("tspan")
                                                       .attr("x", 0)
                                                       .attr("dx", lineHeight + "px")
                                                       .attr("y", 0)
                                                       .attr("dy", yOffset + "px")
                                                       .text(currentNetworkID.substr(1) + ": " + 
                                                             assignedRate.toFixed(3));
                }
            }
        }
    }
}


// When the page loads, open the sidebar.
$(document).ready(function() {
    $('#simple-menu').sidr(side="left");
    jQuery.sidr(method="open")
});

main();

