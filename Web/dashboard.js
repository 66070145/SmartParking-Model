const blynkAuthToken = 'po6V3XG9qb7bnsUw9g21VbVpY7JBvgYo';
const blynkParkingUrl = `https://blynk.cloud/external/api/get?token=${blynkAuthToken}&V0`;
const blynkLightUrl = `https://blynk.cloud/external/api/get?token=${blynkAuthToken}&V1`;
const blynkSliderUrl = `https://blynk.cloud/external/api/get?token=${blynkAuthToken}&V2`;
const blynkSetLightUrl = `https://blynk.cloud/external/api/update?token=${blynkAuthToken}&V1=`;
const blynkSetSliderUrl = `https://blynk.cloud/external/api/update?token=${blynkAuthToken}&V2=`;
let isLightOn = false;
let parkLimit = 3;

async function fetchParkingData() {
    try {
        const response = await fetch(blynkParkingUrl);
        const parkingData = await response.text();
        document.getElementById("parking-slots").textContent = parkingData;
    } catch (error) {
        console.error('Error fetching parking data:', error);
        document.getElementById("parking-slots").textContent = 'Error';
    }
}

document.addEventListener("DOMContentLoaded", () => {
    const toggleButton = document.getElementById("toggle-button");
    let isLightOn = false;

    function updateToggleButton() {
        toggleButton.textContent = isLightOn ? "ON" : "OFF";
        toggleButton.classList.toggle("on", isLightOn);
        toggleButton.classList.toggle("off", !isLightOn);
    }

    function toggleLight() {
        isLightOn = !isLightOn;
        updateToggleButton();
        fetch(blynkSetLightUrl + (isLightOn ? '1' : '0'))
            .catch(error => console.error("Error:", error));
    }

    toggleButton.addEventListener("click", toggleLight);

    // Initial fetch for light status
    async function fetchLightStatus() {
        try {
            const response = await fetch(blynkLightUrl);
            const lightData = await response.text();
            isLightOn = (lightData === "1");
            updateToggleButton();
        } catch (error) {
            console.error('Error fetching light status:', error);
        }
    }
    setInterval(fetchLightStatus, 2000);
    fetchLightStatus();
});

function updateParkLimitFromBlynk() {
    fetch(blynkSliderUrl)
        .then(response => response.text())
        .then(data => {
            parkLimit = parseInt(data);
            const slider = document.getElementById("park-limit-slider");
            const sliderValueDisplay = document.getElementById("slider-value-display");

            slider.value = parkLimit;
            sliderValueDisplay.textContent = parkLimit;
        })
        .catch(error => console.error("Error fetching park_limit:", error));
}

document.addEventListener("DOMContentLoaded", () => {
    const slider = document.getElementById("park-limit-slider");
    const sliderValueDisplay = document.getElementById("slider-value-display");

    // ฟังก์ชันสำหรับส่งค่า slider ไปยัง Blynk
    function sendSliderValueToBlynk(value) {
        fetch(blynkSetSliderUrl + value)
            .then(response => {
                if (response.ok) {
                    console.log(`Slider value ${value} sent to Blynk`);
                } else {
                    console.error("Failed to send slider value to Blynk");
                }
            })
            .catch(error => console.error("Error:", error));
    }

    // การเพิ่ม Event Listener สำหรับ slider
    slider.addEventListener("input", () => {
        sliderValueDisplay.textContent = slider.value; // แสดงค่าของ slider
        sendSliderValueToBlynk(slider.value); // ส่งค่า slider ไปยัง Blynk
    });
});




function fetchData() {
    fetchParkingData();

    updateParkLimitFromBlynk();
}

setInterval(fetchData, 5000);
fetchData();