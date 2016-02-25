# Weight-and-Catapult
<p>
<h1>Weight measuring and Catapult system</h1>
This was a school project where the task was to create with <b>reverse engineering</b> from an old HDD, printer and scener a precise weight measuring device which
is able to shoot an object to the distance at least 20cm.
</p>

<p>
    We used an old <b>HDD Lorentz Actuator</b> which works on principe of Lorentz force. Coil through which flows current is positioned
    in a magnetic field. Based on amplytude and direction of the voltage from the double <b>H-Bridge</b> the HDDs arm moves up and down. The H-Bridge is controlled from
    <b>NXP LPC Expresso</b>. Because through the coil flows current and the wire has some resistance, the coil gets warmer. We measured the temperature at the closest
    place to the coil. Based on this measurements and <b>Kalman Filtering</b> we compensate the errors caused by increase of temperature.
        The weight is measured based on time it takes from first to the second light barrier.
    At the beginning of project we have specified that our device will be able to measure with 1g precision and shoot with precision of 1cm to the distance of 20cm.
    At the beginning of semester we have reached following specifications:
    <ul>
        <li>Weight measuring: 0.1g</li>
        <li>Weight range: [0,20g] </li>
        <li>Shooting: 0.5cm</li>
    </ul>
</p>
