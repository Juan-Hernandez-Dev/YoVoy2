"use client";

import { useState, useEffect } from "react";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import docsConfig from "../docs-config.json";

interface DocConfig {
  titulo: string;
  endpoint: string;
  archivo: string;
}

export default function Home() {
  const [activeTab, setActiveTab] = useState<string>(docsConfig[0].endpoint);
  const [markdownContent, setMarkdownContent] = useState<string>("");
  const [loading, setLoading] = useState<boolean>(true);

  useEffect(() => {
    const loadMarkdown = async () => {
      setLoading(true);
      try {
        const currentDoc = docsConfig.find(
          (doc: DocConfig) => doc.endpoint === activeTab
        );
        if (currentDoc) {
          const response = await fetch(`/api/docs/${activeTab}`);
          const data = await response.json();
          setMarkdownContent(data.content);
        }
      } catch (error) {
        console.error("Error loading markdown:", error);
        setMarkdownContent("# Error\n\nNo se pudo cargar el documento.");
      } finally {
        setLoading(false);
      }
    };

    loadMarkdown();
  }, [activeTab]);

  return (
    <div className="min-h-screen bg-gradient-to-br from-gray-50 to-gray-100">
      {/* Header with Navigation */}
      <header className="bg-white shadow-md sticky top-0 z-50">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-4">
          <div className="flex items-center justify-between">
            <div>
              <h1 className="text-xl font-bold text-gray-900">
                Sistema de Simulación de Tráfico Urbano
              </h1>
              <p className="text-xs text-gray-600 mt-1">
                Documentación Técnica
              </p>
            </div>
          </div>
        </div>

        {/* Navigation Tabs */}
        <nav className="border-t border-gray-200">
          <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8">
            <div className="flex space-x-1 overflow-x-auto py-2">
              {docsConfig.map((doc: DocConfig) => (
                <button
                  key={doc.endpoint}
                  onClick={() => setActiveTab(doc.endpoint)}
                  className={`
                    px-3 py-1.5 text-xs font-medium rounded-lg whitespace-nowrap transition-all duration-200
                    ${
                      activeTab === doc.endpoint
                        ? "bg-blue-600 text-white shadow-md"
                        : "text-gray-700 hover:bg-gray-100 hover:text-gray-900"
                    }
                  `}
                >
                  {doc.titulo}
                </button>
              ))}
            </div>
          </div>
        </nav>
      </header>

      {/* Content */}
      <main className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8">
        <div className="bg-white rounded-lg shadow-lg p-8">
          {loading ? (
            <div className="flex items-center justify-center py-12">
              <div className="animate-spin rounded-full h-12 w-12 border-b-2 border-blue-600"></div>
            </div>
          ) : (
            <article className="prose prose-sm max-w-none">
              <ReactMarkdown
                remarkPlugins={[remarkGfm]}
                components={{
                  h1: ({ node, ...props }) => (
                    <h1
                      className="text-2xl font-bold text-gray-900 mb-4 pb-3 border-b-2 border-gray-300"
                      {...props}
                    />
                  ),
                  h2: ({ node, ...props }) => (
                    <h2
                      className="text-xl font-semibold text-gray-800 mt-6 mb-3"
                      {...props}
                    />
                  ),
                  h3: ({ node, ...props }) => (
                    <h3
                      className="text-lg font-semibold text-gray-800 mt-4 mb-2"
                      {...props}
                    />
                  ),
                  h4: ({ node, ...props }) => (
                    <h4
                      className="text-base font-semibold text-gray-700 mt-3 mb-2"
                      {...props}
                    />
                  ),
                  p: ({ node, ...props }) => (
                    <p
                      className="text-sm text-gray-700 leading-relaxed mb-3"
                      {...props}
                    />
                  ),
                  ul: ({ node, ...props }) => (
                    <ul
                      className="list-disc list-inside space-y-1 mb-3 text-sm text-gray-700"
                      {...props}
                    />
                  ),
                  ol: ({ node, ...props }) => (
                    <ol
                      className="list-decimal list-inside space-y-1 mb-3 text-sm text-gray-700"
                      {...props}
                    />
                  ),
                  li: ({ node, ...props }) => (
                    <li className="ml-4 text-sm" {...props} />
                  ),
                  code: ({ node, inline, ...props }: any) =>
                    inline ? (
                      <code
                        className="bg-gray-100 text-red-600 px-1.5 py-0.5 rounded text-xs font-mono"
                        {...props}
                      />
                    ) : (
                      <code
                        className="block bg-gray-900 text-gray-100 p-3 rounded-lg overflow-x-auto text-xs font-mono"
                        {...props}
                      />
                    ),
                  pre: ({ node, ...props }) => (
                    <pre
                      className="bg-gray-900 text-gray-100 p-3 rounded-lg overflow-x-auto mb-3"
                      {...props}
                    />
                  ),
                  blockquote: ({ node, ...props }) => (
                    <blockquote
                      className="border-l-4 border-blue-500 pl-3 italic text-sm text-gray-600 my-3"
                      {...props}
                    />
                  ),
                  table: ({ node, ...props }) => (
                    <div className="overflow-x-auto mb-3">
                      <table
                        className="min-w-full divide-y divide-gray-200 border border-gray-300 text-xs"
                        {...props}
                      />
                    </div>
                  ),
                  thead: ({ node, ...props }) => (
                    <thead className="bg-gray-50" {...props} />
                  ),
                  tbody: ({ node, ...props }) => (
                    <tbody
                      className="bg-white divide-y divide-gray-200"
                      {...props}
                    />
                  ),
                  tr: ({ node, ...props }) => <tr {...props} />,
                  th: ({ node, ...props }) => (
                    <th
                      className="px-4 py-2 text-left text-xs font-medium text-gray-700 uppercase tracking-wider"
                      {...props}
                    />
                  ),
                  td: ({ node, ...props }) => (
                    <td
                      className="px-4 py-2 whitespace-nowrap text-xs text-gray-700"
                      {...props}
                    />
                  ),
                  a: ({ node, ...props }) => (
                    <a
                      className="text-blue-600 hover:text-blue-800 underline"
                      {...props}
                    />
                  ),
                  hr: ({ node, ...props }) => (
                    <hr
                      className="my-6 border-t-2 border-gray-200"
                      {...props}
                    />
                  ),
                }}
              >
                {markdownContent}
              </ReactMarkdown>
            </article>
          )}
        </div>
      </main>

      {/* Footer */}
      <footer className="bg-white border-t border-gray-200 mt-12">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-6">
          <div className="flex flex-col md:flex-row justify-between items-center">
            <p className="text-sm text-gray-600">
              © 2025 Juan Hernández Dev™ - Simulador de tráfico urbano
            </p>
            <div className="flex space-x-4 mt-4 md:mt-0">
              <a
                href="https://github.com"
                target="_blank"
                rel="noopener noreferrer"
                className="text-gray-600 hover:text-gray-900 transition-colors"
              >
                GitHub
              </a>
            </div>
          </div>
        </div>
      </footer>
    </div>
  );
}
