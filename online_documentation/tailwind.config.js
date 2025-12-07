/** @type {import('tailwindcss').Config} */
module.exports = {
  content: [
    "./pages/**/*.{js,ts,jsx,tsx,mdx}",
    "./components/**/*.{js,ts,jsx,tsx,mdx}",
    "./app/**/*.{js,ts,jsx,tsx,mdx}",
  ],
  theme: {
    extend: {
      fontFamily: {
        poppins: ["Poppins", "sans-serif"],
      },
      typography: {
        DEFAULT: {
          css: {
            maxWidth: "none",
            fontSize: "14px",
            color: "#374151",
            a: {
              color: "#3b82f6",
              "&:hover": {
                color: "#2563eb",
              },
            },
            code: {
              color: "#e11d48",
              backgroundColor: "#f3f4f6",
              padding: "0.25rem 0.375rem",
              borderRadius: "0.25rem",
              fontWeight: "600",
              fontSize: "13px",
            },
            "code::before": {
              content: '""',
            },
            "code::after": {
              content: '""',
            },
            pre: {
              backgroundColor: "#1f2937",
              color: "#f9fafb",
              fontSize: "13px",
            },
          },
        },
      },
    },
  },
  plugins: [require("@tailwindcss/typography")],
};
