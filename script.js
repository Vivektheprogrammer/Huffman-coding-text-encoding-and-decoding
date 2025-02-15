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

function encodeAndSendMessage() {
    const emailInput = document.getElementById("emailInput").value.trim();
    const messageInput = document.getElementById("messageInput").value.trim();
    const statusMessage = document.getElementById("statusMessage");

    // Ensure statusMessage exists before using it
    if (!statusMessage) {
        console.error("❌ Element with ID 'statusMessage' not found.");
        return;
    }

    // Reset previous status message
    statusMessage.textContent = "";
    statusMessage.style.color = "black";

    if (!emailInput || !messageInput) {
        statusMessage.textContent = "⚠️ Please enter both an email and a message.";
        statusMessage.style.color = "red";
        return;
    }

    fetch("http://127.0.0.1:5000/encode", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ message: messageInput })
    })
    .then(response => response.json())
    .then(data => {
        if (!data.encoded_message) throw new Error("Encoding failed.");
        
        return fetch("http://127.0.0.1:5000/encode_and_send", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ email: emailInput, encoded_message: data.encoded_message })
        });
    })
    .then(response => response.json()) 
    .then(data => {
        if (data.success || data.message === "Email sent successfully") {
            statusMessage.textContent = "✅ Email sent successfully!";
            statusMessage.style.color = "green";
        } else {
            statusMessage.textContent = "⚠️ Email sending failed.";
            statusMessage.style.color = "red";
        }
    })
    .catch(error => {
        console.error("Error:", error);
        statusMessage.textContent = "❌ Something went wrong. Please try again.";
        statusMessage.style.color = "red";
    });
}

// Ensure script runs after the page loads
window.onload = function() {
    document.getElementById("sendButton").onclick = encodeAndSendMessage;
};


    

// ✅ Function to handle file selection
function updateFileStatus(event) {
    const file = event.target.files[0];
    if (file) {
        document.getElementById("status").innerText = `Selected: ${file.name}`;
    }
}

// ✅ File Compression function
function compressFile() {
    const fileInput = document.getElementById("fileInput").files[0];
    if (!fileInput) {
        alert("⚠️ Please select a file first.");
        return;
    }

    const formData = new FormData();
    formData.append("file", fileInput);

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
        a.download = fileInput.name + ".huff";
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        alert("✅ File compressed successfully!");
    })
    .catch(error => {
        console.error("Error:", error);
        alert("⚠️ Compression failed. Please try again.");
    });
}

// ✅ File Decompression function
function decompressFile() {
    const fileInput = document.getElementById("fileInput").files[0];
    if (!fileInput) {
        alert("⚠️ Please select a file first.");
        return;
    }

    const formData = new FormData();
    formData.append("file", fileInput);

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
        a.download = fileInput.name.replace(".huff", "");
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        alert("✅ File decompressed successfully!");
    })
    .catch(error => {
        console.error("Error:", error);
        alert("⚠️ Decompression failed. Please try again.");
    });
}

// ✅ Decode function
function decodeMessage() {
    const encodedText = document.getElementById("encodedMessage").value.trim();

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
            document.getElementById("decodedOutput").innerText = "Decoded Message: " + data.decoded_message;
        } else {
            alert("Decoding failed.");
        }
    })
    .catch(error => {
        console.error("Error:", error);
        alert("Something went wrong.");
    });
}
