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
    const message = document.getElementById("messageInput").value;
    const email = document.getElementById("emailInput").value;
    const statusMessage = document.getElementById("statusMessage");

    if (!message || !email) {
        statusMessage.innerText = "Please enter a message and recipient email.";
        return;
    }

    fetch("http://localhost:5000/encode_and_send", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({ message, email })
    })
    .then(response => response.json())
    .then(data => {
        if (data.success) {
            statusMessage.innerHTML = `✅ Message sent successfully! <br><strong>Encoded Message:</strong> ${data.encoded_message}`;
        } else {
            statusMessage.innerText = `❌ Error: ${data.error}`;
        }
    })
    .catch(error => {
        statusMessage.innerText = `❌ Failed to send message: ${error}`;
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