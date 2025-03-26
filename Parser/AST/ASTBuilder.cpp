#include "ASTBuilder.h"

std::shared_ptr<ASTNode> ASTBuilder::createNode(
        const std::string& nonTerminal,
        int /*productionIndex*/,
        const std::vector<std::shared_ptr<ASTNode>>& childNodes
) {
  auto node = std::make_shared<ASTNode>(nonTerminal, false);
  node->children = childNodes;
  return node;
}

std::shared_ptr<ASTNode> ASTBuilder::createTerminal(const std::string& terminalSymbol) {
  return std::make_shared<ASTNode>(terminalSymbol, true);
}
