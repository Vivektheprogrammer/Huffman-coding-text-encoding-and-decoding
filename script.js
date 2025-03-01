document.addEventListener("DOMContentLoaded", function () {
    // Attach event listeners when DOM is loaded
    document.getElementById("fileInput").addEventListener("change", updateFileStatus);
    document.getElementById("progressBar").value = 0;

    const sendButton = document.querySelector("#encoding button");
    if (sendButton) {
        sendButton.addEventListener("click", function (event) {
            event.preventDefault(); // ✅ Prevents page reload
            encodeAndSendMessage();
        });
    }
});

function decodeMessage() {
    const encodedText = document.getElementById("encodedMessage").value.trim();
    const decodedOutput = document.getElementById("decodedOutput");

    if (!encodedText) {
        alert("⚠️ Please enter an encoded message.");
        return;
    }

    fetch("http://127.0.0.1:5000/decode", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ encoded_message: encodedText })
    })
    .then(response => response.json())
    .then(data => {
        if (data.decoded_message) {
            decodedOutput.innerText = "Decoded Message: " + data.decoded_message;
            decodedOutput.style.display = "block";  // ✅ Ensure visibility
        } else {
            decodedOutput.innerText = "Decoding failed.";
            decodedOutput.style.display = "block";  // ✅ Show even if failed
            alert("Decoding failed.");
        }
    })
    .catch(error => {
        console.error("Error:", error);
        alert("Something went wrong.");
    });
}


// ✅ Function to handle file selection
function updateFileStatus(event) {
    const file = event.target.files[0];
    if (file) {
        document.getElementById("status").innerText = `Selected: ${file.name}`;
    }
}

// ✅ File Compression function
function compressFile() {
    const fileInput = document.getElementById("fileInput");
    if (!fileInput.files.length) {
        alert("⚠️ Please select a file first.");
        return;
    }

    const formData = new FormData();
    formData.append("file", fileInput.files[0]);

    const progressBar = document.getElementById("progressBar");
    progressBar.value = 10;

    fetch("http://127.0.0.1:5000/compress", {
        method: "POST",
        body: formData
    })
    .then(response => response.blob())
    .then(blob => {
        progressBar.value = 100; // ✅ Progress bar update
        const url = window.URL.createObjectURL(blob);
        const a = document.createElement("a");
        a.href = url;
        a.download = fileInput.files[0].name + ".huff";
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        alert("✅ File compressed successfully!");

        // ✅ Clear file input
        fileInput.value = "";
        document.getElementById("status").innerText = "No file selected";
    })
    .catch(error => {
        console.error("Error:", error);
        alert("⚠️ Compression failed. Please try again.");
    });
}

// ✅ File Decompression function
function decompressFile() {
    const fileInput = document.getElementById("fileInput");
    if (!fileInput.files.length) {
        alert("⚠️ Please select a file first.");
        return;
    }

    const formData = new FormData();
    formData.append("file", fileInput.files[0]);

    const progressBar = document.getElementById("progressBar");
    progressBar.value = 10;

    fetch("http://127.0.0.1:5000/decompress", {
        method: "POST",
        body: formData
    })
    .then(response => response.blob())
    .then(blob => {
        progressBar.value = 100; // ✅ Progress bar update
        const url = window.URL.createObjectURL(blob);
        const a = document.createElement("a");
        a.href = url;
        a.download = fileInput.files[0].name.replace(".huff", "");
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        alert("✅ File decompressed successfully!");

        // ✅ Clear file input
        fileInput.value = "";
        document.getElementById("status").innerText = "No file selected";
    })
    .catch(error => {
        console.error("Error:", error);
        alert("⚠️ Decompression failed. Please try again.");
    });
}
function encodeAndSendMessage() {
    const recipientEmail = document.getElementById("emailInput").value.trim();
    const message = document.getElementById("messageInput").value.trim();
    const statusMessage = document.getElementById("statusMessage");
    const encodedOutput = document.getElementById("encodedOutput"); // <p> element

    if (!message || !recipientEmail) {
        alert("⚠️ Please enter both a message and recipient email.");
        return;
    }

    fetch("http://127.0.0.1:5000/encode_and_send", {
        method: "POST",
        mode: "cors",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            email: recipientEmail,
            message: message  // Send the original message for encoding
        })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            alert("✅ Encoded message sent successfully!");
            statusMessage.innerText = "Message sent successfully!";

            // Display the encoded message in the <p> element
            encodedOutput.innerText = "" + data.encoded_message;
            encodedOutput.style.display = "block"; // Ensure the <p> is visible
        } else {
            alert("⚠️ Error sending message: " + data.error);
            statusMessage.innerText = "Error: " + data.error;
        }
    })
    .catch(error => {
        console.error("Fetch Error:", error);
        alert("⚠️ Network error. Check server logs.");
    });
}

